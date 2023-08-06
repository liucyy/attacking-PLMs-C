static void nbd_coroutine_start(NBDClientSession *s,

                                NBDRequest *request)

{

    

    if (s->in_flight == MAX_NBD_REQUESTS) {

        qemu_co_queue_wait(&s->free_sema, NULL);

        assert(s->in_flight < MAX_NBD_REQUESTS);

    }

    s->in_flight++;



    

}