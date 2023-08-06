static int qdm2_get_vlc (GetBitContext *gb, VLC *vlc, int flag, int depth)

{

    int value;



    value = get_vlc2(gb, vlc->table, vlc->bits, depth);



    

    if (value-- == 0)

        value = get_bits (gb, get_bits (gb, 3) + 1);



    

    if (flag) {

        int tmp = vlc_stage3_values[value];



        if ((value & ~3) > 0)

            tmp += get_bits (gb, (value >> 2));

        value = tmp;

    }



    return value;

}