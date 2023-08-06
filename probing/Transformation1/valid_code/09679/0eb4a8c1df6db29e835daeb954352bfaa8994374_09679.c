static int qcow2_create2(const char *filename, int64_t total_size,

                         const char *backing_file, const char *backing_format,

                         int flags, size_t cluster_size, PreallocMode prealloc,

                         QemuOpts *opts, int version, int refcount_order,

                         const char *encryptfmt, Error **errp)

{

    int cluster_bits;

    QDict *options;



    

    cluster_bits = ctz32(cluster_size);

    if (cluster_bits < MIN_CLUSTER_BITS || cluster_bits > MAX_CLUSTER_BITS ||

        (1 << cluster_bits) != cluster_size)

    {

        error_setg(errp, "Cluster size must be a power of two between %d and "

                   "%dk", 1 << MIN_CLUSTER_BITS, 1 << (MAX_CLUSTER_BITS - 10));

        return -EINVAL;

    }



    

    BlockBackend *blk;

    QCowHeader *header;

    uint64_t* refcount_table;

    Error *local_err = NULL;

    int ret;



    if (prealloc == PREALLOC_MODE_FULL || prealloc == PREALLOC_MODE_FALLOC) {

        int64_t prealloc_size =

            qcow2_calc_prealloc_size(total_size, cluster_size, refcount_order);

        qemu_opt_set_number(opts, BLOCK_OPT_SIZE, prealloc_size, &error_abort);

        qemu_opt_set(opts, BLOCK_OPT_PREALLOC, PreallocMode_lookup[prealloc],

                     &error_abort);

    }



    ret = bdrv_create_file(filename, opts, &local_err);

    if (ret < 0) {

        error_propagate(errp, local_err);

        return ret;

    }



    blk = blk_new_open(filename, NULL, NULL,

                       BDRV_O_RDWR | BDRV_O_RESIZE | BDRV_O_PROTOCOL,

                       &local_err);

    if (blk == NULL) {

        error_propagate(errp, local_err);

        return -EIO;

    }



    blk_set_allow_write_beyond_eof(blk, true);



    

    QEMU_BUILD_BUG_ON((1 << MIN_CLUSTER_BITS) < sizeof(*header));

    header = g_malloc0(cluster_size);

    *header = (QCowHeader) {

        .magic                      = cpu_to_be32(QCOW_MAGIC),

        .version                    = cpu_to_be32(version),

        .cluster_bits               = cpu_to_be32(cluster_bits),

        .size                       = cpu_to_be64(0),

        .l1_table_offset            = cpu_to_be64(0),

        .l1_size                    = cpu_to_be32(0),

        .refcount_table_offset      = cpu_to_be64(cluster_size),

        .refcount_table_clusters    = cpu_to_be32(1),

        .refcount_order             = cpu_to_be32(refcount_order),

        .header_length              = cpu_to_be32(sizeof(*header)),

    };



    

    header->crypt_method = cpu_to_be32(QCOW_CRYPT_NONE);



    if (flags & BLOCK_FLAG_LAZY_REFCOUNTS) {

        header->compatible_features |=

            cpu_to_be64(QCOW2_COMPAT_LAZY_REFCOUNTS);

    }



    ret = blk_pwrite(blk, 0, header, cluster_size, 0);

    g_free(header);

    if (ret < 0) {

        error_setg_errno(errp, -ret, "Could not write qcow2 header");

        goto out;

    }



    

    refcount_table = g_malloc0(2 * cluster_size);

    refcount_table[0] = cpu_to_be64(2 * cluster_size);

    ret = blk_pwrite(blk, cluster_size, refcount_table, 2 * cluster_size, 0);

    g_free(refcount_table);



    if (ret < 0) {

        error_setg_errno(errp, -ret, "Could not write refcount table");

        goto out;

    }



    blk_unref(blk);

    blk = NULL;



    

    options = qdict_new();

    qdict_put_str(options, "driver", "qcow2");

    blk = blk_new_open(filename, NULL, options,

                       BDRV_O_RDWR | BDRV_O_RESIZE | BDRV_O_NO_FLUSH,

                       &local_err);

    if (blk == NULL) {

        error_propagate(errp, local_err);

        ret = -EIO;

        goto out;

    }



    ret = qcow2_alloc_clusters(blk_bs(blk), 3 * cluster_size);

    if (ret < 0) {

        error_setg_errno(errp, -ret, "Could not allocate clusters for qcow2 "

                         "header and refcount table");

        goto out;



    } else if (ret != 0) {

        error_report("Huh, first cluster in empty image is already in use?");

        abort();

    }



    

    ret = qcow2_update_header(blk_bs(blk));

    if (ret < 0) {

        error_setg_errno(errp, -ret, "Could not update qcow2 header");

        goto out;

    }



    

    ret = blk_truncate(blk, total_size, errp);

    if (ret < 0) {

        error_prepend(errp, "Could not resize image: ");

        goto out;

    }



    

    if (backing_file) {

        ret = bdrv_change_backing_file(blk_bs(blk), backing_file, backing_format);

        if (ret < 0) {

            error_setg_errno(errp, -ret, "Could not assign backing file '%s' "

                             "with format '%s'", backing_file, backing_format);

            goto out;

        }

    }



    

    if (encryptfmt) {

        ret = qcow2_set_up_encryption(blk_bs(blk), encryptfmt, opts, errp);

        if (ret < 0) {

            goto out;

        }

    }



    

    if (prealloc != PREALLOC_MODE_OFF) {

        BDRVQcow2State *s = blk_bs(blk)->opaque;

        qemu_co_mutex_lock(&s->lock);

        ret = preallocate(blk_bs(blk));

        qemu_co_mutex_unlock(&s->lock);

        if (ret < 0) {

            error_setg_errno(errp, -ret, "Could not preallocate metadata");

            goto out;

        }

    }



    blk_unref(blk);

    blk = NULL;



    

    options = qdict_new();

    qdict_put_str(options, "driver", "qcow2");

    blk = blk_new_open(filename, NULL, options,

                       BDRV_O_RDWR | BDRV_O_NO_BACKING | BDRV_O_NO_IO,

                       &local_err);

    if (blk == NULL) {

        error_propagate(errp, local_err);

        ret = -EIO;

        goto out;

    }



    ret = 0;

out:

    if (blk) {

        blk_unref(blk);

    }

    return ret;

}