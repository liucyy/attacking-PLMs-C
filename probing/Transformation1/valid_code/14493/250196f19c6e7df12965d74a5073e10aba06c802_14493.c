int qcow2_alloc_cluster_link_l2(BlockDriverState *bs, QCowL2Meta *m)

{

    BDRVQcowState *s = bs->opaque;

    int i, j = 0, l2_index, ret;

    uint64_t *old_cluster, start_sect, l2_offset, *l2_table;

    uint64_t cluster_offset = m->cluster_offset;

    bool cow = false;



    trace_qcow2_cluster_link_l2(qemu_coroutine_self(), m->nb_clusters);



    if (m->nb_clusters == 0)

        return 0;



    old_cluster = g_malloc(m->nb_clusters * sizeof(uint64_t));



    

    start_sect = (m->offset & ~(s->cluster_size - 1)) >> 9;

    if (m->n_start) {

        cow = true;

        qemu_co_mutex_unlock(&s->lock);

        ret = copy_sectors(bs, start_sect, cluster_offset, 0, m->n_start);

        qemu_co_mutex_lock(&s->lock);

        if (ret < 0)

            goto err;

    }



    if (m->nb_available & (s->cluster_sectors - 1)) {

        uint64_t end = m->nb_available & ~(uint64_t)(s->cluster_sectors - 1);

        cow = true;

        qemu_co_mutex_unlock(&s->lock);

        ret = copy_sectors(bs, start_sect + end, cluster_offset + (end << 9),

                m->nb_available - end, s->cluster_sectors);

        qemu_co_mutex_lock(&s->lock);

        if (ret < 0)

            goto err;

    }



    

    if (cow) {

        qcow2_cache_depends_on_flush(s->l2_table_cache);

    }



    qcow2_cache_set_dependency(bs, s->l2_table_cache, s->refcount_block_cache);

    ret = get_cluster_table(bs, m->offset, &l2_table, &l2_offset, &l2_index);

    if (ret < 0) {

        goto err;

    }

    qcow2_cache_entry_mark_dirty(s->l2_table_cache, l2_table);



    for (i = 0; i < m->nb_clusters; i++) {

        

        if(l2_table[l2_index + i] != 0)

            old_cluster[j++] = l2_table[l2_index + i];



        l2_table[l2_index + i] = cpu_to_be64((cluster_offset +

                    (i << s->cluster_bits)) | QCOW_OFLAG_COPIED);

     }





    ret = qcow2_cache_put(bs, s->l2_table_cache, (void**) &l2_table);

    if (ret < 0) {

        goto err;

    }



    

    if (j != 0) {

        for (i = 0; i < j; i++) {

            qcow2_free_any_clusters(bs,

                be64_to_cpu(old_cluster[i]) & ~QCOW_OFLAG_COPIED, 1);

        }

    }



    ret = 0;

err:

    g_free(old_cluster);

    return ret;

 }