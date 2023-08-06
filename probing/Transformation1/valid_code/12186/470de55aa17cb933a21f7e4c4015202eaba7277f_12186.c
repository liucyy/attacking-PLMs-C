static inline int check_for_slice(AVSContext *h) {

    GetBitContext *gb = &h->s.gb;

    int align;



    if(h->mbx)

        return 0;

    align = (-get_bits_count(gb)) & 7;

    

    if(!align && (show_bits(gb,8) == 0x80))

        get_bits(gb,8);

    if((show_bits_long(gb,24+align) & 0xFFFFFF) == 0x000001) {

        skip_bits_long(gb,24+align);

        h->stc = get_bits(gb,8);

        decode_slice_header(h,gb);

        return 1;

    }

    return 0;

}