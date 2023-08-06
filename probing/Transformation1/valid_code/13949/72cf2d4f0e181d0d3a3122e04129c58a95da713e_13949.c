void qemu_aio_wait(void)

{

    int ret;



    if (qemu_bh_poll())

        return;



    do {

        AioHandler *node;

        fd_set rdfds, wrfds;

        int max_fd = -1;



        walking_handlers = 1;



        FD_ZERO(&rdfds);

        FD_ZERO(&wrfds);



        

        LIST_FOREACH(node, &aio_handlers, node) {

            

            if (node->io_flush && node->io_flush(node->opaque) == 0)

                continue;



            if (!node->deleted && node->io_read) {

                FD_SET(node->fd, &rdfds);

                max_fd = MAX(max_fd, node->fd + 1);

            }

            if (!node->deleted && node->io_write) {

                FD_SET(node->fd, &wrfds);

                max_fd = MAX(max_fd, node->fd + 1);

            }

        }



        walking_handlers = 0;



        

        if (max_fd == -1)

            break;



        

        ret = select(max_fd, &rdfds, &wrfds, NULL, NULL);

        if (ret == -1 && errno == EINTR)

            continue;



        

        if (ret > 0) {

            walking_handlers = 1;



            

            node = LIST_FIRST(&aio_handlers);

            while (node) {

                AioHandler *tmp;



                if (!node->deleted &&

                    FD_ISSET(node->fd, &rdfds) &&

                    node->io_read) {

                    node->io_read(node->opaque);

                }

                if (!node->deleted &&

                    FD_ISSET(node->fd, &wrfds) &&

                    node->io_write) {

                    node->io_write(node->opaque);

                }



                tmp = node;

                node = LIST_NEXT(node, node);



                if (tmp->deleted) {

                    LIST_REMOVE(tmp, node);

                    qemu_free(tmp);

                }

            }



            walking_handlers = 0;

        }

    } while (ret == 0);

}