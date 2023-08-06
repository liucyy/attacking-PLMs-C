static int raw_open_common(BlockDriverState *bs, const char *filename,

                           int bdrv_flags, int open_flags)

{

    BDRVRawState *s = bs->opaque;

    int fd, ret;



    ret = raw_normalize_devicepath(&filename);

    if (ret != 0) {

        return ret;

    }



    s->open_flags = open_flags | O_BINARY;

    s->open_flags &= ~O_ACCMODE;

    if (bdrv_flags & BDRV_O_RDWR) {

        s->open_flags |= O_RDWR;

    } else {

        s->open_flags |= O_RDONLY;

    }



    

    if ((bdrv_flags & BDRV_O_NOCACHE))

        s->open_flags |= O_DIRECT;

    if (!(bdrv_flags & BDRV_O_CACHE_WB))

        s->open_flags |= O_DSYNC;



    s->fd = -1;

    fd = qemu_open(filename, s->open_flags, 0644);

    if (fd < 0) {

        ret = -errno;

        if (ret == -EROFS)

            ret = -EACCES;

        return ret;

    }

    s->fd = fd;

    s->aligned_buf = NULL;



    if ((bdrv_flags & BDRV_O_NOCACHE)) {

        

        s->aligned_buf_size = 32 * MAX_BLOCKSIZE;

        s->aligned_buf = qemu_memalign(MAX_BLOCKSIZE, s->aligned_buf_size);

        if (s->aligned_buf == NULL) {

            goto out_close;

        }

    }



    

    if (paio_init() < 0) {

        goto out_free_buf;

    }



#ifdef CONFIG_LINUX_AIO

    

    if ((bdrv_flags & (BDRV_O_NOCACHE|BDRV_O_NATIVE_AIO)) ==

                      (BDRV_O_NOCACHE|BDRV_O_NATIVE_AIO)) {



        s->aio_ctx = laio_init();

        if (!s->aio_ctx) {

            goto out_free_buf;

        }

        s->use_aio = 1;

    } else

#endif

    {

#ifdef CONFIG_LINUX_AIO

        s->use_aio = 0;

#endif

    }



#ifdef CONFIG_XFS

    if (platform_test_xfs_fd(s->fd)) {

        s->is_xfs = 1;

    }

#endif



    return 0;



out_free_buf:

    qemu_vfree(s->aligned_buf);

out_close:

    qemu_close(fd);

    return -errno;

}