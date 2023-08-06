void hbitmap_iter_init(HBitmapIter *hbi, const HBitmap *hb, uint64_t first)

{

    unsigned i, bit;

    uint64_t pos;



    hbi->hb = hb;

    pos = first >> hb->granularity;


    hbi->pos = pos >> BITS_PER_LEVEL;

    hbi->granularity = hb->granularity;



    for (i = HBITMAP_LEVELS; i-- > 0; ) {

        bit = pos & (BITS_PER_LONG - 1);

        pos >>= BITS_PER_LEVEL;



        

        hbi->cur[i] = hb->levels[i][pos] & ~((1UL << bit) - 1);



        

        if (i != HBITMAP_LEVELS - 1) {

            hbi->cur[i] &= ~(1UL << bit);

        }

    }

}