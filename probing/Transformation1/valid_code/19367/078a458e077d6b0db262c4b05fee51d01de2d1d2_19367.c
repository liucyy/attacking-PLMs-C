static inline int64_t get_sector_offset(BlockDriverState *bs,

    int64_t sector_num, int write)

{

    BDRVVPCState *s = bs->opaque;

    uint64_t offset = sector_num * 512;

    uint64_t bitmap_offset, block_offset;

    uint32_t pagetable_index, pageentry_index;



    pagetable_index = offset / s->block_size;

    pageentry_index = (offset % s->block_size) / 512;



    if (pagetable_index >= s->max_table_entries || s->pagetable[pagetable_index] == 0xffffffff)

        return -1; 



    bitmap_offset = 512 * (uint64_t) s->pagetable[pagetable_index];

    block_offset = bitmap_offset + s->bitmap_size + (512 * pageentry_index);



    

    

    

    

    

    if (write && (s->last_bitmap_offset != bitmap_offset)) {

        uint8_t bitmap[s->bitmap_size];



        s->last_bitmap_offset = bitmap_offset;

        memset(bitmap, 0xff, s->bitmap_size);

        bdrv_pwrite(bs->file, bitmap_offset, bitmap, s->bitmap_size);

    }













#if 0

#ifdef CACHE

    if (bitmap_offset != s->last_bitmap)

    {

	lseek(s->fd, bitmap_offset, SEEK_SET);



	s->last_bitmap = bitmap_offset;



	

	

	read(s->fd, s->pageentry_u8, 512);

	for (i = 0; i < 128; i++)

	    be32_to_cpus(&s->pageentry_u32[i]);

    }



    if ((s->pageentry_u8[pageentry_index / 8] >> (pageentry_index % 8)) & 1)

	return -1;

#else

    lseek(s->fd, bitmap_offset + (pageentry_index / 8), SEEK_SET);



    read(s->fd, &bitmap_entry, 1);



    if ((bitmap_entry >> (pageentry_index % 8)) & 1)

	return -1; 

#endif

#endif



    return block_offset;

}