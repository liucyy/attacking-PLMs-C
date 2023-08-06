static coroutine_fn void nbd_read_reply_entry(void *opaque)

{

    NBDClientSession *s = opaque;

    uint64_t i;

    int ret;



    for (;;) {

        assert(s->reply.handle == 0);

        ret = nbd_receive_reply(s->ioc, &s->reply);

        if (ret < 0) {

            break;

        }



        

        i = HANDLE_TO_INDEX(s, s->reply.handle);

        if (i >= MAX_NBD_REQUESTS || !s->recv_coroutine[i]) {

            break;

        }



        

        aio_co_wake(s->recv_coroutine[i]);

        qemu_coroutine_yield();

    }

    s->read_reply_co = NULL;

}