static void bochs_refresh_limits(BlockDriverState *bs, Error **errp)

{

    bs->request_alignment = BDRV_SECTOR_SIZE; 

}