static void raw_probe_alignment(BlockDriverState *bs, int fd, Error **errp)

{

    BDRVRawState *s = bs->opaque;

    char *buf;

    size_t max_align = MAX(MAX_BLOCKSIZE, getpagesize());



    

    if (bdrv_is_sg(bs) || !s->needs_alignment) {

        bs->request_alignment = 1;

        s->buf_align = 1;

        return;

    }



    bs->request_alignment = 0;

    s->buf_align = 0;

    

    if (probe_logical_blocksize(fd, &bs->request_alignment) < 0) {

        bs->request_alignment = 0;

    }

#ifdef CONFIG_XFS

    if (s->is_xfs) {

        struct dioattr da;

        if (xfsctl(NULL, fd, XFS_IOC_DIOINFO, &da) >= 0) {

            bs->request_alignment = da.d_miniosz;

            

            

        }

    }

#endif



    

    if (!s->buf_align) {

        size_t align;

        buf = qemu_memalign(max_align, 2 * max_align);

        for (align = 512; align <= max_align; align <<= 1) {

            if (raw_is_io_aligned(fd, buf + align, max_align)) {

                s->buf_align = align;

                break;

            }

        }

        qemu_vfree(buf);

    }



    if (!bs->request_alignment) {

        size_t align;

        buf = qemu_memalign(s->buf_align, max_align);

        for (align = 512; align <= max_align; align <<= 1) {

            if (raw_is_io_aligned(fd, buf, align)) {

                bs->request_alignment = align;

                break;

            }

        }

        qemu_vfree(buf);

    }



    if (!s->buf_align || !bs->request_alignment) {

        error_setg(errp, "Could not find working O_DIRECT alignment. "

                         "Try cache.direct=off.");

    }

}