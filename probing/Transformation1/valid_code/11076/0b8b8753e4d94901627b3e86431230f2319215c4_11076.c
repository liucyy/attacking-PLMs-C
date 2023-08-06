static void nbd_recv_coroutines_enter_all(NbdClientSession *s)

{

    int i;



    for (i = 0; i < MAX_NBD_REQUESTS; i++) {

        if (s->recv_coroutine[i]) {

            qemu_coroutine_enter(s->recv_coroutine[i], NULL);

        }

    }

}