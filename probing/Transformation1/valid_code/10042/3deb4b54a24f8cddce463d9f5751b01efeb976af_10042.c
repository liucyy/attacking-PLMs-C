static int parse_packet_header(WMAVoiceContext *s)

{

    GetBitContext *gb = &s->gb;

    unsigned int res;



    if (get_bits_left(gb) < 11)

        return 1;

    skip_bits(gb, 4);          

    s->has_residual_lsps = get_bits1(gb);

    do {

        res = get_bits(gb, 6); 

                               

        if (get_bits_left(gb) < 6 * (res == 0x3F) + s->spillover_bitsize)

            return 1;

    } while (res == 0x3F);

    s->spillover_nbits   = get_bits(gb, s->spillover_bitsize);



    return 0;

}