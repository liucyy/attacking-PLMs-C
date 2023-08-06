static int make_completely_empty(BlockDriverState *bs)

{

    BDRVQcow2State *s = bs->opaque;

    int ret, l1_clusters;

    int64_t offset;

    uint64_t *new_reftable = NULL;

    uint64_t rt_entry, l1_size2;

    struct {

        uint64_t l1_offset;

        uint64_t reftable_offset;

        uint32_t reftable_clusters;

    } QEMU_PACKED l1_ofs_rt_ofs_cls;



    ret = qcow2_cache_empty(bs, s->l2_table_cache);

    if (ret < 0) {

        goto fail;

    }



    ret = qcow2_cache_empty(bs, s->refcount_block_cache);

    if (ret < 0) {

        goto fail;

    }



    

    ret = qcow2_mark_dirty(bs);

    if (ret < 0) {

        goto fail;

    }



    BLKDBG_EVENT(bs->file, BLKDBG_L1_UPDATE);



    l1_clusters = DIV_ROUND_UP(s->l1_size, s->cluster_size / sizeof(uint64_t));

    l1_size2 = (uint64_t)s->l1_size * sizeof(uint64_t);



    



    ret = bdrv_pwrite_zeroes(bs->file, s->l1_table_offset,

                             l1_clusters * s->cluster_size, 0);

    if (ret < 0) {

        goto fail_broken_refcounts;

    }

    memset(s->l1_table, 0, l1_size2);



    BLKDBG_EVENT(bs->file, BLKDBG_EMPTY_IMAGE_PREPARE);



    

    ret = bdrv_pwrite_zeroes(bs->file, s->cluster_size,

                             (2 + l1_clusters) * s->cluster_size, 0);

    

    if (ret < 0) {

        goto fail_broken_refcounts;

    }



    BLKDBG_EVENT(bs->file, BLKDBG_L1_UPDATE);

    BLKDBG_EVENT(bs->file, BLKDBG_REFTABLE_UPDATE);



    

    l1_ofs_rt_ofs_cls.l1_offset = cpu_to_be64(3 * s->cluster_size);

    l1_ofs_rt_ofs_cls.reftable_offset = cpu_to_be64(s->cluster_size);

    l1_ofs_rt_ofs_cls.reftable_clusters = cpu_to_be32(1);

    ret = bdrv_pwrite_sync(bs->file, offsetof(QCowHeader, l1_table_offset),

                           &l1_ofs_rt_ofs_cls, sizeof(l1_ofs_rt_ofs_cls));

    if (ret < 0) {

        goto fail_broken_refcounts;

    }



    s->l1_table_offset = 3 * s->cluster_size;



    new_reftable = g_try_new0(uint64_t, s->cluster_size / sizeof(uint64_t));

    if (!new_reftable) {

        ret = -ENOMEM;

        goto fail_broken_refcounts;

    }



    s->refcount_table_offset = s->cluster_size;

    s->refcount_table_size   = s->cluster_size / sizeof(uint64_t);




    g_free(s->refcount_table);

    s->refcount_table = new_reftable;

    new_reftable = NULL;



    



    BLKDBG_EVENT(bs->file, BLKDBG_REFBLOCK_ALLOC);



    

    rt_entry = cpu_to_be64(2 * s->cluster_size);

    ret = bdrv_pwrite_sync(bs->file, s->cluster_size,

                           &rt_entry, sizeof(rt_entry));

    if (ret < 0) {

        goto fail_broken_refcounts;

    }

    s->refcount_table[0] = 2 * s->cluster_size;



    s->free_cluster_index = 0;

    assert(3 + l1_clusters <= s->refcount_block_size);

    offset = qcow2_alloc_clusters(bs, 3 * s->cluster_size + l1_size2);

    if (offset < 0) {

        ret = offset;

        goto fail_broken_refcounts;

    } else if (offset > 0) {

        error_report("First cluster in emptied image is in use");

        abort();

    }



    

    ret = qcow2_mark_clean(bs);

    if (ret < 0) {

        goto fail;

    }



    ret = bdrv_truncate(bs->file->bs, (3 + l1_clusters) * s->cluster_size);

    if (ret < 0) {

        goto fail;

    }



    return 0;



fail_broken_refcounts:

    

    bs->drv = NULL;



fail:

    g_free(new_reftable);

    return ret;

}