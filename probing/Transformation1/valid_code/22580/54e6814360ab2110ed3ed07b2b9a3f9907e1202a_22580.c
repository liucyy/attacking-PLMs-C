int qcow2_alloc_cluster_offset(BlockDriverState *bs, uint64_t offset,

    int n_start, int n_end, int *num, QCowL2Meta *m)

{

    BDRVQcowState *s = bs->opaque;

    int l2_index, ret, sectors;

    uint64_t *l2_table;

    unsigned int nb_clusters, keep_clusters;

    uint64_t cluster_offset;



    trace_qcow2_alloc_clusters_offset(qemu_coroutine_self(), offset,

                                      n_start, n_end);



    

again:

    ret = get_cluster_table(bs, offset, &l2_table, &l2_index);

    if (ret < 0) {

        return ret;

    }



    

    nb_clusters = MIN(size_to_clusters(s, n_end << BDRV_SECTOR_BITS),

                      s->l2_size - l2_index);



    cluster_offset = be64_to_cpu(l2_table[l2_index]);



    

    if (qcow2_get_cluster_type(cluster_offset) == QCOW2_CLUSTER_NORMAL

        && (cluster_offset & QCOW_OFLAG_COPIED))

    {

        

        keep_clusters =

            count_contiguous_clusters(nb_clusters, s->cluster_size,

                                      &l2_table[l2_index], 0,

                                      QCOW_OFLAG_COPIED | QCOW_OFLAG_ZERO);

        assert(keep_clusters <= nb_clusters);

        nb_clusters -= keep_clusters;

    } else {

        

        if (cluster_offset & QCOW_OFLAG_COMPRESSED) {

            nb_clusters = 1;

        } else {

            nb_clusters = count_cow_clusters(s, nb_clusters, l2_table, l2_index);

        }



        keep_clusters = 0;

        cluster_offset = 0;

    }



    cluster_offset &= L2E_OFFSET_MASK;



    

    ret = qcow2_cache_put(bs, s->l2_table_cache, (void**) &l2_table);

    if (ret < 0) {

        return ret;

    }



    

    *m = (QCowL2Meta) {

        .cluster_offset     = cluster_offset,

        .nb_clusters        = 0,

    };

    qemu_co_queue_init(&m->dependent_requests);



    if (nb_clusters > 0) {

        uint64_t alloc_offset;

        uint64_t alloc_cluster_offset;

        uint64_t keep_bytes = keep_clusters * s->cluster_size;



        

        alloc_offset = offset + keep_bytes;



        if (keep_clusters == 0) {

            alloc_cluster_offset = 0;

        } else {

            alloc_cluster_offset = cluster_offset + keep_bytes;

        }



        

        ret = do_alloc_cluster_offset(bs, alloc_offset, &alloc_cluster_offset,

                                      &nb_clusters);

        if (ret == -EAGAIN) {

            goto again;

        } else if (ret < 0) {

            goto fail;

        }



        

        if (nb_clusters > 0) {

            int requested_sectors = n_end - keep_clusters * s->cluster_sectors;

            int avail_sectors = (keep_clusters + nb_clusters)

                                << (s->cluster_bits - BDRV_SECTOR_BITS);



            *m = (QCowL2Meta) {

                .cluster_offset = keep_clusters == 0 ?

                                  alloc_cluster_offset : cluster_offset,

                .alloc_offset   = alloc_cluster_offset,

                .offset         = alloc_offset,

                .n_start        = keep_clusters == 0 ? n_start : 0,

                .nb_clusters    = nb_clusters,

                .nb_available   = MIN(requested_sectors, avail_sectors),

            };

            qemu_co_queue_init(&m->dependent_requests);

            QLIST_INSERT_HEAD(&s->cluster_allocs, m, next_in_flight);

        }

    }



    

    sectors = (keep_clusters + nb_clusters) << (s->cluster_bits - 9);

    if (sectors > n_end) {

        sectors = n_end;

    }



    assert(sectors > n_start);

    *num = sectors - n_start;



    return 0;



fail:

    if (m->nb_clusters > 0) {

        QLIST_REMOVE(m, next_in_flight);

    }

    return ret;

}