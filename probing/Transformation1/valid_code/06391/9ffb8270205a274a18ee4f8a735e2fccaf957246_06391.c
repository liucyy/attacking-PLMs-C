int qio_channel_readv_all(QIOChannel *ioc,

                          const struct iovec *iov,

                          size_t niov,

                          Error **errp)

{

    int ret = -1;

    struct iovec *local_iov = g_new(struct iovec, niov);

    struct iovec *local_iov_head = local_iov;

    unsigned int nlocal_iov = niov;



    nlocal_iov = iov_copy(local_iov, nlocal_iov,

                          iov, niov,

                          0, iov_size(iov, niov));



    while (nlocal_iov > 0) {

        ssize_t len;

        len = qio_channel_readv(ioc, local_iov, nlocal_iov, errp);

        if (len == QIO_CHANNEL_ERR_BLOCK) {

            qio_channel_wait(ioc, G_IO_IN);

            continue;

        } else if (len < 0) {

            goto cleanup;

        } else if (len == 0) {

            error_setg(errp,

                       "Unexpected end-of-file before all bytes were read");

            goto cleanup;

        }



        iov_discard_front(&local_iov, &nlocal_iov, len);

    }



    ret = 0;



 cleanup:

    g_free(local_iov_head);

    return ret;

}