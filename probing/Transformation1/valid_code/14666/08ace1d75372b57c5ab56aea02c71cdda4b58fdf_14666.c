static coroutine_fn void nbd_read_reply_entry(void *opaque)

{

    NBDClientSession *s = opaque;

    uint64_t i;

    int ret = 0;

    Error *local_err = NULL;



    while (!s->quit) {

        assert(s->reply.handle == 0);

        ret = nbd_receive_reply(s->ioc, &s->reply, &local_err);

        if (ret < 0) {

            error_report_err(local_err);

        }

        if (ret <= 0) {

            break;

        }



        

        i = HANDLE_TO_INDEX(s, s->reply.handle);

        if (i >= MAX_NBD_REQUESTS ||

            !s->requests[i].coroutine ||

            !s->requests[i].receiving ||

            (nbd_reply_is_structured(&s->reply) && !s->info.structured_reply))

        {

            break;

        }



        

        aio_co_wake(s->requests[i].coroutine);

        qemu_coroutine_yield();

    }



    s->quit = true;

    nbd_recv_coroutines_wake_all(s);

    s->read_reply_co = NULL;

}