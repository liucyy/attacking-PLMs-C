static void decode_nal_sei_decoded_picture_hash(HEVCContext *s)

{

    int cIdx, i;

    uint8_t hash_type;

    

    

    GetBitContext *gb = &s->HEVClc->gb;

    hash_type = get_bits(gb, 8);



    for (cIdx = 0; cIdx < 3; cIdx++) {

        if (hash_type == 0) {

            s->is_md5 = 1;

            for (i = 0; i < 16; i++)

                s->md5[cIdx][i] = get_bits(gb, 8);

        } else if (hash_type == 1) {

            

            skip_bits(gb, 16);

        } else if (hash_type == 2) {

            

            skip_bits(gb, 32);

        }

    }

}