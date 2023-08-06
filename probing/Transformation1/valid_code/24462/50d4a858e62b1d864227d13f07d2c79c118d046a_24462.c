static ssize_t handle_aiocb_rw(RawPosixAIOData *aiocb)

{

    ssize_t nbytes;

    char *buf;



    if (!(aiocb->aio_type & QEMU_AIO_MISALIGNED)) {

        

        if (aiocb->aio_niov == 1) {

             return handle_aiocb_rw_linear(aiocb, aiocb->aio_iov->iov_base);

        }

        

        if (preadv_present) {

            nbytes = handle_aiocb_rw_vector(aiocb);

            if (nbytes == aiocb->aio_nbytes ||

                (nbytes < 0 && nbytes != -ENOSYS)) {

                return nbytes;

            }

            preadv_present = false;

        }



        

    }



    

    buf = qemu_blockalign(aiocb->bs, aiocb->aio_nbytes);

    if (aiocb->aio_type & QEMU_AIO_WRITE) {

        char *p = buf;

        int i;



        for (i = 0; i < aiocb->aio_niov; ++i) {

            memcpy(p, aiocb->aio_iov[i].iov_base, aiocb->aio_iov[i].iov_len);

            p += aiocb->aio_iov[i].iov_len;

        }

        assert(p - buf == aiocb->aio_nbytes);

    }



    nbytes = handle_aiocb_rw_linear(aiocb, buf);

    if (!(aiocb->aio_type & QEMU_AIO_WRITE)) {

        char *p = buf;

        size_t count = aiocb->aio_nbytes, copy;

        int i;



        for (i = 0; i < aiocb->aio_niov && count; ++i) {

            copy = count;

            if (copy > aiocb->aio_iov[i].iov_len) {

                copy = aiocb->aio_iov[i].iov_len;

            }

            memcpy(aiocb->aio_iov[i].iov_base, p, copy);

            assert(count >= copy);

            p     += copy;

            count -= copy;

        }

        assert(count == 0);

    }

    qemu_vfree(buf);



    return nbytes;

}