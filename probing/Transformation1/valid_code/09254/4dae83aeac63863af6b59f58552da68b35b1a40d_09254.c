static void glib_select_fill(int *max_fd, fd_set *rfds, fd_set *wfds,

                             fd_set *xfds, struct timeval *tv)

{

    GMainContext *context = g_main_context_default();

    int i;

    int timeout = 0, cur_timeout;



    g_main_context_prepare(context, &max_priority);



    n_poll_fds = g_main_context_query(context, max_priority, &timeout,

                                      poll_fds, ARRAY_SIZE(poll_fds));

    g_assert(n_poll_fds <= ARRAY_SIZE(poll_fds));



    for (i = 0; i < n_poll_fds; i++) {

        GPollFD *p = &poll_fds[i];



        if ((p->events & G_IO_IN)) {

            FD_SET(p->fd, rfds);

            *max_fd = MAX(*max_fd, p->fd);

        }

        if ((p->events & G_IO_OUT)) {

            FD_SET(p->fd, wfds);

            *max_fd = MAX(*max_fd, p->fd);

        }

        if ((p->events & G_IO_ERR)) {

            FD_SET(p->fd, xfds);

            *max_fd = MAX(*max_fd, p->fd);

        }

    }



    cur_timeout = (tv->tv_sec * 1000) + ((tv->tv_usec + 500) / 1000);

    if (timeout >= 0 && timeout < cur_timeout) {

        tv->tv_sec = timeout / 1000;

        tv->tv_usec = (timeout % 1000) * 1000;

    }

}