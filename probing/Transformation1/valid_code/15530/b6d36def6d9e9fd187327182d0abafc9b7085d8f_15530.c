static int discard_single_l2(BlockDriverState *bs, uint64_t offset,

    unsigned int nb_clusters, enum qcow2_discard_type type, bool full_discard)

{

    BDRVQcow2State *s = bs->opaque;

    uint64_t *l2_table;

    int l2_index;

    int ret;

    int i;



    ret = get_cluster_table(bs, offset, &l2_table, &l2_index);

    if (ret < 0) {

        return ret;

    }



    

    nb_clusters = MIN(nb_clusters, s->l2_size - l2_index);



    for (i = 0; i < nb_clusters; i++) {

        uint64_t old_l2_entry;



        old_l2_entry = be64_to_cpu(l2_table[l2_index + i]);



        

        switch (qcow2_get_cluster_type(old_l2_entry)) {

            case QCOW2_CLUSTER_UNALLOCATED:

                if (full_discard || !bs->backing_hd) {

                    continue;

                }

                break;



            case QCOW2_CLUSTER_ZERO:

                if (!full_discard) {

                    continue;

                }

                break;



            case QCOW2_CLUSTER_NORMAL:

            case QCOW2_CLUSTER_COMPRESSED:

                break;



            default:

                abort();

        }



        

        qcow2_cache_entry_mark_dirty(bs, s->l2_table_cache, l2_table);

        if (!full_discard && s->qcow_version >= 3) {

            l2_table[l2_index + i] = cpu_to_be64(QCOW_OFLAG_ZERO);

        } else {

            l2_table[l2_index + i] = cpu_to_be64(0);

        }



        

        qcow2_free_any_clusters(bs, old_l2_entry, 1, type);

    }



    qcow2_cache_put(bs, s->l2_table_cache, (void **) &l2_table);



    return nb_clusters;

}