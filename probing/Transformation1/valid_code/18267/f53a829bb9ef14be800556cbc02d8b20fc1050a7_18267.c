static void nbd_reply_ready(void *opaque)

{

    NbdClientSession *s = opaque;

    uint64_t i;

    int ret;



    if (s->reply.handle == 0) {

        

        ret = nbd_receive_reply(s->sock, &s->reply);

        if (ret == -EAGAIN) {

            return;

        }

        if (ret < 0) {

            s->reply.handle = 0;

            goto fail;

        }

    }



    

    i = HANDLE_TO_INDEX(s, s->reply.handle);

    if (i >= MAX_NBD_REQUESTS) {

        goto fail;

    }



    if (s->recv_coroutine[i]) {

        qemu_coroutine_enter(s->recv_coroutine[i], NULL);

        return;

    }



fail:

    nbd_teardown_connection(s);

}