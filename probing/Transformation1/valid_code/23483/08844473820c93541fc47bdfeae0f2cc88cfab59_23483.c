void bdrv_setup_io_funcs(BlockDriver *bdrv)

{

    

    if (!bdrv->bdrv_co_readv) {

        bdrv->bdrv_co_readv = bdrv_co_readv_em;

        bdrv->bdrv_co_writev = bdrv_co_writev_em;



        

        if (!bdrv->bdrv_aio_readv) {

            

            bdrv->bdrv_aio_readv = bdrv_aio_readv_em;

            bdrv->bdrv_aio_writev = bdrv_aio_writev_em;

        }

    }

}