int qcow2_update_header(BlockDriverState *bs)

{

    BDRVQcowState *s = bs->opaque;

    QCowHeader *header;

    char *buf;

    size_t buflen = s->cluster_size;

    int ret;

    uint64_t total_size;

    uint32_t refcount_table_clusters;

    size_t header_length;

    Qcow2UnknownHeaderExtension *uext;



    buf = qemu_blockalign(bs, buflen);



    

    header = (QCowHeader*) buf;



    if (buflen < sizeof(*header)) {

        ret = -ENOSPC;

        goto fail;

    }



    header_length = sizeof(*header) + s->unknown_header_fields_size;

    total_size = bs->total_sectors * BDRV_SECTOR_SIZE;

    refcount_table_clusters = s->refcount_table_size >> (s->cluster_bits - 3);



    *header = (QCowHeader) {

        

        .magic                  = cpu_to_be32(QCOW_MAGIC),

        .version                = cpu_to_be32(s->qcow_version),

        .backing_file_offset    = 0,

        .backing_file_size      = 0,

        .cluster_bits           = cpu_to_be32(s->cluster_bits),

        .size                   = cpu_to_be64(total_size),

        .crypt_method           = cpu_to_be32(s->crypt_method_header),

        .l1_size                = cpu_to_be32(s->l1_size),

        .l1_table_offset        = cpu_to_be64(s->l1_table_offset),

        .refcount_table_offset  = cpu_to_be64(s->refcount_table_offset),

        .refcount_table_clusters = cpu_to_be32(refcount_table_clusters),

        .nb_snapshots           = cpu_to_be32(s->nb_snapshots),

        .snapshots_offset       = cpu_to_be64(s->snapshots_offset),



        

        .incompatible_features  = cpu_to_be64(s->incompatible_features),

        .compatible_features    = cpu_to_be64(s->compatible_features),

        .autoclear_features     = cpu_to_be64(s->autoclear_features),

        .refcount_order         = cpu_to_be32(3 + REFCOUNT_SHIFT),

        .header_length          = cpu_to_be32(header_length),

    };



    

    switch (s->qcow_version) {

    case 2:

        ret = offsetof(QCowHeader, incompatible_features);

        break;

    case 3:

        ret = sizeof(*header);

        break;

    default:

        return -EINVAL;

    }



    buf += ret;

    buflen -= ret;

    memset(buf, 0, buflen);



    

    if (s->unknown_header_fields_size) {

        if (buflen < s->unknown_header_fields_size) {

            ret = -ENOSPC;

            goto fail;

        }



        memcpy(buf, s->unknown_header_fields, s->unknown_header_fields_size);

        buf += s->unknown_header_fields_size;

        buflen -= s->unknown_header_fields_size;

    }



    

    if (*bs->backing_format) {

        ret = header_ext_add(buf, QCOW2_EXT_MAGIC_BACKING_FORMAT,

                             bs->backing_format, strlen(bs->backing_format),

                             buflen);

        if (ret < 0) {

            goto fail;

        }



        buf += ret;

        buflen -= ret;

    }



    

    Qcow2Feature features[] = {

        

    };



    ret = header_ext_add(buf, QCOW2_EXT_MAGIC_FEATURE_TABLE,

                         features, sizeof(features), buflen);

    if (ret < 0) {

        goto fail;

    }

    buf += ret;

    buflen -= ret;



    

    QLIST_FOREACH(uext, &s->unknown_header_ext, next) {

        ret = header_ext_add(buf, uext->magic, uext->data, uext->len, buflen);

        if (ret < 0) {

            goto fail;

        }



        buf += ret;

        buflen -= ret;

    }



    

    ret = header_ext_add(buf, QCOW2_EXT_MAGIC_END, NULL, 0, buflen);

    if (ret < 0) {

        goto fail;

    }



    buf += ret;

    buflen -= ret;



    

    if (*bs->backing_file) {

        size_t backing_file_len = strlen(bs->backing_file);



        if (buflen < backing_file_len) {

            ret = -ENOSPC;

            goto fail;

        }



        strncpy(buf, bs->backing_file, buflen);



        header->backing_file_offset = cpu_to_be64(buf - ((char*) header));

        header->backing_file_size   = cpu_to_be32(backing_file_len);

    }



    

    ret = bdrv_pwrite(bs->file, 0, header, s->cluster_size);

    if (ret < 0) {

        goto fail;

    }



    ret = 0;

fail:

    qemu_vfree(header);

    return ret;

}