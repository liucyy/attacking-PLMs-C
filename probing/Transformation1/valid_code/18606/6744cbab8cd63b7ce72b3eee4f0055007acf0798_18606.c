static int qcow2_create2(const char *filename, int64_t total_size,

                         const char *backing_file, const char *backing_format,

                         int flags, size_t cluster_size, int prealloc,

                         QEMUOptionParameter *options)

{

    

    int cluster_bits;

    cluster_bits = ffs(cluster_size) - 1;

    if (cluster_bits < MIN_CLUSTER_BITS || cluster_bits > MAX_CLUSTER_BITS ||

        (1 << cluster_bits) != cluster_size)

    {

        error_report(

            "Cluster size must be a power of two between %d and %dk",

            1 << MIN_CLUSTER_BITS, 1 << (MAX_CLUSTER_BITS - 10));

        return -EINVAL;

    }



    

    BlockDriverState* bs;

    QCowHeader header;

    uint8_t* refcount_table;

    int ret;



    ret = bdrv_create_file(filename, options);

    if (ret < 0) {

        return ret;

    }



    ret = bdrv_file_open(&bs, filename, BDRV_O_RDWR);

    if (ret < 0) {

        return ret;

    }



    

    memset(&header, 0, sizeof(header));

    header.magic = cpu_to_be32(QCOW_MAGIC);

    header.version = cpu_to_be32(QCOW_VERSION);

    header.cluster_bits = cpu_to_be32(cluster_bits);

    header.size = cpu_to_be64(0);

    header.l1_table_offset = cpu_to_be64(0);

    header.l1_size = cpu_to_be32(0);

    header.refcount_table_offset = cpu_to_be64(cluster_size);

    header.refcount_table_clusters = cpu_to_be32(1);



    if (flags & BLOCK_FLAG_ENCRYPT) {

        header.crypt_method = cpu_to_be32(QCOW_CRYPT_AES);

    } else {

        header.crypt_method = cpu_to_be32(QCOW_CRYPT_NONE);

    }



    ret = bdrv_pwrite(bs, 0, &header, sizeof(header));

    if (ret < 0) {

        goto out;

    }



    

    refcount_table = g_malloc0(cluster_size);

    ret = bdrv_pwrite(bs, cluster_size, refcount_table, cluster_size);

    g_free(refcount_table);



    if (ret < 0) {

        goto out;

    }



    bdrv_close(bs);



    

    BlockDriver* drv = bdrv_find_format("qcow2");

    assert(drv != NULL);

    ret = bdrv_open(bs, filename,

        BDRV_O_RDWR | BDRV_O_CACHE_WB | BDRV_O_NO_FLUSH, drv);

    if (ret < 0) {

        goto out;

    }



    ret = qcow2_alloc_clusters(bs, 2 * cluster_size);

    if (ret < 0) {

        goto out;



    } else if (ret != 0) {

        error_report("Huh, first cluster in empty image is already in use?");

        abort();

    }



    

    ret = bdrv_truncate(bs, total_size * BDRV_SECTOR_SIZE);

    if (ret < 0) {

        goto out;

    }



    

    if (backing_file) {

        ret = bdrv_change_backing_file(bs, backing_file, backing_format);

        if (ret < 0) {

            goto out;

        }

    }



    

    if (prealloc) {

        ret = preallocate(bs);

        if (ret < 0) {

            goto out;

        }

    }



    ret = 0;

out:

    bdrv_delete(bs);

    return ret;

}