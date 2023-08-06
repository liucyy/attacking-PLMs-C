static int coroutine_fn bdrv_co_do_pwrite_zeroes(BlockDriverState *bs,

    int64_t offset, int count, BdrvRequestFlags flags)

{

    BlockDriver *drv = bs->drv;

    QEMUIOVector qiov;

    struct iovec iov = {0};

    int ret = 0;

    bool need_flush = false;

    int head = 0;

    int tail = 0;



    int max_write_zeroes = MIN_NON_ZERO(bs->bl.max_pwrite_zeroes, INT_MAX);

    int alignment = MAX(bs->bl.pwrite_zeroes_alignment,

                        bs->bl.request_alignment);



    assert(alignment % bs->bl.request_alignment == 0);

    head = offset % alignment;

    tail = (offset + count) % alignment;

    max_write_zeroes = QEMU_ALIGN_DOWN(max_write_zeroes, alignment);

    assert(max_write_zeroes >= bs->bl.request_alignment);



    while (count > 0 && !ret) {

        int num = count;



        

        if (head) {

            

            num = MIN(count, alignment - head);

            head = 0;

        } else if (tail && num > alignment) {

            

            num -= tail;

        }



        

        if (num > max_write_zeroes) {

            num = max_write_zeroes;

        }



        ret = -ENOTSUP;

        

        if (drv->bdrv_co_pwrite_zeroes) {

            ret = drv->bdrv_co_pwrite_zeroes(bs, offset, num,

                                             flags & bs->supported_zero_flags);

            if (ret != -ENOTSUP && (flags & BDRV_REQ_FUA) &&

                !(bs->supported_zero_flags & BDRV_REQ_FUA)) {

                need_flush = true;

            }

        } else {

            assert(!bs->supported_zero_flags);

        }



        if (ret == -ENOTSUP) {

            

            int max_transfer = MIN_NON_ZERO(bs->bl.max_transfer,

                                            MAX_WRITE_ZEROES_BOUNCE_BUFFER);

            BdrvRequestFlags write_flags = flags & ~BDRV_REQ_ZERO_WRITE;



            if ((flags & BDRV_REQ_FUA) &&

                !(bs->supported_write_flags & BDRV_REQ_FUA)) {

                

                write_flags &= ~BDRV_REQ_FUA;

                need_flush = true;

            }

            num = MIN(num, max_transfer);

            iov.iov_len = num;

            if (iov.iov_base == NULL) {

                iov.iov_base = qemu_try_blockalign(bs, num);

                if (iov.iov_base == NULL) {

                    ret = -ENOMEM;

                    goto fail;

                }

                memset(iov.iov_base, 0, num);

            }

            qemu_iovec_init_external(&qiov, &iov, 1);



            ret = bdrv_driver_pwritev(bs, offset, num, &qiov, write_flags);



            

            if (num < max_transfer) {

                qemu_vfree(iov.iov_base);

                iov.iov_base = NULL;

            }

        }



        offset += num;

        count -= num;

    }



fail:

    if (ret == 0 && need_flush) {

        ret = bdrv_co_flush(bs);

    }

    qemu_vfree(iov.iov_base);

    return ret;

}