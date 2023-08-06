static inline uint64_t bdrv_get_align(BlockDriverState *bs)

{

    

    return MAX(BDRV_SECTOR_SIZE, bs->request_alignment);

}