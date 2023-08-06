int qcow2_alloc_cluster_offset(BlockDriverState *bs, uint64_t offset,

    int n_start, int n_end, int *num, uint64_t *host_offset, QCowL2Meta **m)

{

    BDRVQcowState *s = bs->opaque;

    uint64_t start, remaining;

    uint64_t cluster_offset;

    uint64_t cur_bytes;

    int ret;



    trace_qcow2_alloc_clusters_offset(qemu_coroutine_self(), offset,

                                      n_start, n_end);



    assert(n_start * BDRV_SECTOR_SIZE == offset_into_cluster(s, offset));

    offset = start_of_cluster(s, offset);



again:

    start = offset + (n_start << BDRV_SECTOR_BITS);

    remaining = (n_end - n_start) << BDRV_SECTOR_BITS;

    cluster_offset = 0;

    *host_offset = 0;



    

    cur_bytes = remaining;

    ret = handle_dependencies(bs, start, &cur_bytes);

    if (ret == -EAGAIN) {

        goto again;

    } else if (ret < 0) {

        return ret;

    } else {

        

    }



    

    ret = handle_copied(bs, start, &cluster_offset, &cur_bytes, m);

    if (ret < 0) {

        return ret;

    } else if (ret) {

        if (!*host_offset) {

            *host_offset = start_of_cluster(s, cluster_offset);

        }



        start           += cur_bytes;

        remaining       -= cur_bytes;

        cluster_offset  += cur_bytes;



        cur_bytes = remaining;

    } else if (cur_bytes == 0) {

        goto done;

    }



    

    if (remaining == 0) {

        goto done;

    }



    

    ret = handle_alloc(bs, start, &cluster_offset, &cur_bytes, m);

    if (ret < 0) {

        return ret;

    } else if (ret) {

        if (!*host_offset) {

            *host_offset = start_of_cluster(s, cluster_offset);

        }



        start           += cur_bytes;

        remaining       -= cur_bytes;

        cluster_offset  += cur_bytes;

    }



    

done:

    *num = (n_end - n_start) - (remaining >> BDRV_SECTOR_BITS);

    assert(*num > 0);

    assert(*host_offset != 0);



    return 0;

}