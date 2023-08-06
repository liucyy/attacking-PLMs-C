void aio_set_fd_handler(AioContext *ctx,

                        int fd,

                        IOHandler *io_read,

                        IOHandler *io_write,

                        AioFlushHandler *io_flush,

                        void *opaque)

{

    AioHandler *node;



    node = find_aio_handler(ctx, fd);



    

    if (!io_read && !io_write) {

        if (node) {

            

            if (ctx->walking_handlers)

                node->deleted = 1;

            else {

                

                QLIST_REMOVE(node, node);

                g_free(node);

            }

        }

    } else {

        if (node == NULL) {

            

            node = g_malloc0(sizeof(AioHandler));

            node->fd = fd;

            QLIST_INSERT_HEAD(&ctx->aio_handlers, node, node);

        }

        

        node->io_read = io_read;

        node->io_write = io_write;

        node->io_flush = io_flush;

        node->opaque = opaque;

    }

}