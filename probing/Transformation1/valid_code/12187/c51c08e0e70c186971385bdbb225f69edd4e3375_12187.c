static int decode_display_orientation(H264Context *h)

{

    h->sei_display_orientation_present = !get_bits1(&h->gb);



    if (h->sei_display_orientation_present) {

        h->sei_hflip = get_bits1(&h->gb);     

        h->sei_vflip = get_bits1(&h->gb);     



        h->sei_anticlockwise_rotation = get_bits(&h->gb, 16);

        get_ue_golomb(&h->gb);  

        skip_bits1(&h->gb);     

    }



    return 0;

}