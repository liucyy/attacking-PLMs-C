static void nbd_coroutine_start(NbdClientSession *s,

   struct nbd_request *request)

{

    

    if (s->in_flight >= MAX_NBD_REQUESTS - 1) {

        qemu_co_mutex_lock(&s->free_sema);

        assert(s->in_flight < MAX_NBD_REQUESTS);

    }

    s->in_flight++;



    

}