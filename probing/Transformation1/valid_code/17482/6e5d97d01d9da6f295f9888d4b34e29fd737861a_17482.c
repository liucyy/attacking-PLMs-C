void qemu_aio_flush(void)

{

    AioHandler *node;

    int ret;



    do {

        ret = 0;



	

        qemu_aio_wait();



        LIST_FOREACH(node, &aio_handlers, node) {

            ret |= node->io_flush(node->opaque);

        }

    } while (ret > 0);

}