static bool coroutine_fn wait_serialising_requests(BdrvTrackedRequest *self)

{

    BlockDriverState *bs = self->bs;

    BdrvTrackedRequest *req;

    bool retry;

    bool waited = false;



    if (!bs->serialising_in_flight) {

        return false;

    }



    do {

        retry = false;

        QLIST_FOREACH(req, &bs->tracked_requests, list) {

            if (req == self || (!req->serialising && !self->serialising)) {

                continue;

            }

            if (tracked_request_overlaps(req, self->overlap_offset,

                                         self->overlap_bytes))

            {

                

                assert(qemu_coroutine_self() != req->co);



                

                if (!req->waiting_for) {

                    self->waiting_for = req;

                    qemu_co_queue_wait(&req->wait_queue);

                    self->waiting_for = NULL;

                    retry = true;

                    waited = true;

                    break;

                }

            }

        }

    } while (retry);



    return waited;

}