static ssize_t unix_writev_buffer(void *opaque, struct iovec *iov, int iovcnt,

                                  int64_t pos)

{

    QEMUFileSocket *s = opaque;

    ssize_t len, offset;

    ssize_t size = iov_size(iov, iovcnt);

    ssize_t total = 0;



    assert(iovcnt > 0);

    offset = 0;

    while (size > 0) {

        

        while (offset >= iov[0].iov_len) {

            offset -= iov[0].iov_len;

            iov++, iovcnt--;

        }



        

        assert(iovcnt > 0);

        iov[0].iov_base += offset;

        iov[0].iov_len -= offset;



        do {

            len = writev(s->fd, iov, iovcnt);

        } while (len == -1 && errno == EINTR);

        if (len == -1) {

            return -errno;

        }



        

        iov[0].iov_base -= offset;

        iov[0].iov_len += offset;



        

        offset += len;

        total += len;

        size -= len;

    }



    return total;

}