int qemu_poll_ns(GPollFD *fds, guint nfds, int64_t timeout)

{

#ifdef CONFIG_PPOLL

    if (timeout < 0) {

        return ppoll((struct pollfd *)fds, nfds, NULL, NULL);

    } else {

        struct timespec ts;

        int64_t tvsec = timeout / 1000000000LL;

        

        if (tvsec > (int64_t)INT32_MAX) {

            tvsec = INT32_MAX;

        }

        ts.tv_sec = tvsec;

        ts.tv_nsec = timeout % 1000000000LL;

        return ppoll((struct pollfd *)fds, nfds, &ts, NULL);

    }

#else

    return g_poll(fds, nfds, qemu_timeout_ns_to_ms(timeout));

#endif

}