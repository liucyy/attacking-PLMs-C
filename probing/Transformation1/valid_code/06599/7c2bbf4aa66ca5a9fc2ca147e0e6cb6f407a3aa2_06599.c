static int preallocate(BlockDriverState *bs)

{

    uint64_t nb_sectors;

    uint64_t offset;

    uint64_t host_offset = 0;

    int num;

    int ret;

    QCowL2Meta *meta;



    nb_sectors = bdrv_getlength(bs) >> 9;

    offset = 0;



    while (nb_sectors) {

        num = MIN(nb_sectors, INT_MAX >> 9);

        ret = qcow2_alloc_cluster_offset(bs, offset, &num,

                                         &host_offset, &meta);

        if (ret < 0) {

            return ret;

        }



        ret = qcow2_alloc_cluster_link_l2(bs, meta);

        if (ret < 0) {

            qcow2_free_any_clusters(bs, meta->alloc_offset, meta->nb_clusters,

                                    QCOW2_DISCARD_NEVER);

            return ret;

        }



        

        if (meta != NULL) {

            QLIST_REMOVE(meta, next_in_flight);

        }



        



        nb_sectors -= num;

        offset += num << 9;

    }



    

    if (host_offset != 0) {

        uint8_t buf[512];

        memset(buf, 0, 512);

        ret = bdrv_write(bs->file, (host_offset >> 9) + num - 1, buf, 1);

        if (ret < 0) {

            return ret;

        }

    }



    return 0;

}