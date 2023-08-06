int qemu_aio_set_fd_handler(int fd,

                            IOHandler *io_read,

                            IOHandler *io_write,

                            AioFlushHandler *io_flush,

                            void *opaque)

{

    AioHandler *node;



    node = find_aio_handler(fd);



    

    if (!io_read && !io_write) {

        if (node) {

            

            if (walking_handlers)

                node->deleted = 1;

            else {

                

                LIST_REMOVE(node, node);

                qemu_free(node);

            }

        }

    } else {

        if (node == NULL) {

            

            node = qemu_mallocz(sizeof(AioHandler));

            node->fd = fd;

            LIST_INSERT_HEAD(&aio_handlers, node, node);

        }

        

        node->io_read = io_read;

        node->io_write = io_write;

        node->io_flush = io_flush;

        node->opaque = opaque;

    }



    qemu_set_fd_handler2(fd, NULL, io_read, io_write, opaque);



    return 0;

}