static int h261_decode_gob_header(H261Context *h){

    unsigned int val;

    MpegEncContext * const s = &h->s;

    

    

    val = show_bits(&s->gb, 15);

    if(val)

        return -1;



    

    skip_bits(&s->gb, 16);



    h->gob_number = get_bits(&s->gb, 4); 

    s->qscale = get_bits(&s->gb, 5); 



    

    while (get_bits1(&s->gb) != 0) {

        skip_bits(&s->gb, 8);

    }



    if(s->qscale==0)

        return -1;



    

    

    

    h->current_mba = 0;

    h->mba_diff = 0;



    return 0;

}