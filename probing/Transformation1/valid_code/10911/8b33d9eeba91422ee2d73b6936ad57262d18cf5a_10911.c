static int check_write_unsafe(BlockDriverState *bs, int64_t sector_num,

                              const uint8_t *buf, int nb_sectors)

{

    

    if (!bs->probed) {

        return 0;

    }



    if (sector_num == 0 && nb_sectors > 0) {

        return check_for_block_signature(bs, buf);

    }



    return 0;

}