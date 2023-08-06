static void qemu_rbd_aio_event_reader(void *opaque)

{

    BDRVRBDState *s = opaque;



    ssize_t ret;



    do {

        char *p = (char *)&s->event_rcb;



        

        ret = read(s->fds[RBD_FD_READ], p + s->event_reader_pos,

                   sizeof(s->event_rcb) - s->event_reader_pos);

        if (ret > 0) {

            s->event_reader_pos += ret;

            if (s->event_reader_pos == sizeof(s->event_rcb)) {

                s->event_reader_pos = 0;

                qemu_rbd_complete_aio(s->event_rcb);

            }

        }

    } while (ret < 0 && errno == EINTR);

}