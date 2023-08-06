static void decode_nal_sei_frame_packing_arrangement(HEVCContext *s)

{

    GetBitContext *gb = &s->HEVClc->gb;

    int cancel, type, quincunx, content;



    get_ue_golomb(gb);                  

    cancel = get_bits1(gb);             

    if (cancel == 0) {

        type     = get_bits(gb, 7);     

        quincunx = get_bits1(gb);       

        content  = get_bits(gb, 6);     



        

        

        

        skip_bits(gb, 6);



        if (quincunx == 0 && type != 5)

            skip_bits(gb, 16);  

        skip_bits(gb, 8);       

        skip_bits1(gb);         

    }

    skip_bits1(gb);             



    s->sei_frame_packing_present      = (cancel == 0);

    s->frame_packing_arrangement_type = type;

    s->content_interpretation_type    = content;

    s->quincunx_subsampling           = quincunx;

}