int raw_get_aio_fd(BlockDriverState *bs)

{

    BDRVRawState *s;



    if (!bs->drv) {

        return -ENOMEDIUM;

    }



    if (bs->drv == bdrv_find_format("raw")) {

        bs = bs->file;

    }



    

    if (bs->drv->bdrv_aio_readv != raw_aio_readv) {

        return -ENOTSUP;

    }



    s = bs->opaque;

    if (!s->use_aio) {

        return -ENOTSUP;

    }

    return s->fd;

}