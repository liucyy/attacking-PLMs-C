static int vhdx_allocate_block(BlockDriverState *bs, BDRVVHDXState *s,

                                    uint64_t *new_offset)

{

    *new_offset = bdrv_getlength(bs->file->bs);



    

    *new_offset = ROUND_UP(*new_offset, 1024 * 1024);



    return bdrv_truncate(bs->file, *new_offset + s->block_size,

                         PREALLOC_MODE_OFF, NULL);

}