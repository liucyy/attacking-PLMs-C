static int decode_nal_sei_frame_packing_arrangement(HEVCContext *s)

{

    GetBitContext *gb = &s->HEVClc->gb;



    get_ue_golomb(gb);                  

    s->sei_frame_packing_present = !get_bits1(gb);



    if (s->sei_frame_packing_present) {

        s->frame_packing_arrangement_type = get_bits(gb, 7);

        s->quincunx_subsampling           = get_bits1(gb);

        s->content_interpretation_type    = get_bits(gb, 6);



        

        

        

        skip_bits(gb, 6);



        if (!s->quincunx_subsampling && s->frame_packing_arrangement_type != 5)

            skip_bits(gb, 16);  

        skip_bits(gb, 8);       

        skip_bits1(gb);         

    }

    skip_bits1(gb);             

    return 0;

}