static void coroutine_fn wait_for_overlapping_requests(BlockDriverState *bs,

        int64_t offset, unsigned int bytes)

{

    BdrvTrackedRequest *req;

    int64_t cluster_offset;

    unsigned int cluster_bytes;

    bool retry;



    

    round_bytes_to_clusters(bs, offset, bytes, &cluster_offset, &cluster_bytes);



    do {

        retry = false;

        QLIST_FOREACH(req, &bs->tracked_requests, list) {

            if (tracked_request_overlaps(req, cluster_offset, cluster_bytes)) {

                

                assert(qemu_coroutine_self() != req->co);



                qemu_co_queue_wait(&req->wait_queue);

                retry = true;

                break;

            }

        }

    } while (retry);

}