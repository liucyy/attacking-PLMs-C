static bool is_zero(BlockDriverState *bs, int64_t offset, int64_t bytes)

{

    int nr;

    int64_t res;

    int64_t start;



    

    start = QEMU_ALIGN_DOWN(offset, BDRV_SECTOR_SIZE);

    bytes = QEMU_ALIGN_UP(offset + bytes, BDRV_SECTOR_SIZE) - start;



    

    if (start + bytes > bs->total_sectors * BDRV_SECTOR_SIZE) {

        bytes = bs->total_sectors * BDRV_SECTOR_SIZE - start;

    }



    if (!bytes) {

        return true;

    }

    res = bdrv_get_block_status_above(bs, NULL, start >> BDRV_SECTOR_BITS,

                                      bytes >> BDRV_SECTOR_BITS, &nr, NULL);

    return res >= 0 && (res & BDRV_BLOCK_ZERO) &&

        nr * BDRV_SECTOR_SIZE == bytes;

}