static void raw_probe_alignment(BlockDriverState *bs, int fd, Error **errp)

{

    BDRVRawState *s = bs->opaque;

    char *buf;



    

    if (bs->sg || !s->needs_alignment) {

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

        buf = qemu_memalign(MAX_BLOCKSIZE, 2 * MAX_BLOCKSIZE);

        for (align = 512; align <= MAX_BLOCKSIZE; align <<= 1) {

            if (pread(fd, buf + align, MAX_BLOCKSIZE, 0) >= 0) {

                s->buf_align = align;

                break;

            }

        }

        qemu_vfree(buf);

    }



    if (!bs->request_alignment) {

        size_t align;

        buf = qemu_memalign(s->buf_align, MAX_BLOCKSIZE);

        for (align = 512; align <= MAX_BLOCKSIZE; align <<= 1) {

            if (pread(fd, buf, align, 0) >= 0) {

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