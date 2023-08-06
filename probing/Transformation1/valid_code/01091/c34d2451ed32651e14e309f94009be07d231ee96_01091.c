static inline int seek_to_sector(BlockDriverState *bs, int64_t sector_num)

{

    BDRVParallelsState *s = bs->opaque;

    uint32_t index, offset, position;



    index = sector_num / s->tracks;

    offset = sector_num % s->tracks;



    

    if ((index > s->catalog_size) || (s->catalog_bitmap[index] == 0))

	return -1;



    position = (s->catalog_bitmap[index] + offset) * 512;









    if (lseek(s->fd, position, SEEK_SET) != position)

	return -1;



    return 0;

}