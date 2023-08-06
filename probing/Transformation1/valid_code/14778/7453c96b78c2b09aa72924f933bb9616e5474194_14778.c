static int64_t alloc_clusters_imrt(BlockDriverState *bs,

                                   int cluster_count,

                                   uint16_t **refcount_table,

                                   int64_t *imrt_nb_clusters,

                                   int64_t *first_free_cluster)

{

    BDRVQcowState *s = bs->opaque;

    int64_t cluster = *first_free_cluster, i;

    bool first_gap = true;

    int contiguous_free_clusters;

    int ret;



    

    for (contiguous_free_clusters = 0;

         cluster < *imrt_nb_clusters &&

         contiguous_free_clusters < cluster_count;

         cluster++)

    {

        if (!(*refcount_table)[cluster]) {

            contiguous_free_clusters++;

            if (first_gap) {

                

                *first_free_cluster = cluster;

                first_gap = false;

            }

        } else if (contiguous_free_clusters) {

            contiguous_free_clusters = 0;

        }

    }



    



    

    if (contiguous_free_clusters < cluster_count) {

        

        ret = realloc_refcount_array(s, refcount_table, imrt_nb_clusters,

                                     cluster + cluster_count

                                     - contiguous_free_clusters);

        if (ret < 0) {

            return ret;

        }

    }



    

    cluster -= contiguous_free_clusters;

    for (i = 0; i < cluster_count; i++) {

        (*refcount_table)[cluster + i] = 1;

    }



    return cluster << s->cluster_bits;

}