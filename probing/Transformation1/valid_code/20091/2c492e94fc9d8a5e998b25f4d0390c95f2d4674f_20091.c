static void mpeg1_skip_picture(MpegEncContext *s, int pict_num)

{

    assert(s->codec_id == CODEC_ID_MPEG1VIDEO); 



    

    put_header(s, PICTURE_START_CODE);

    

    put_bits(&s->pb, 10, pict_num & 0x3ff); 

    

    put_bits(&s->pb, 3, P_TYPE);

    put_bits(&s->pb, 16, 0xffff); 

    

    put_bits(&s->pb, 1, 1); 

    put_bits(&s->pb, 3, 1); 

    

    put_bits(&s->pb, 1, 0); 

    

    

    put_header(s, SLICE_MIN_START_CODE);

    put_bits(&s->pb, 5, 1); 

    put_bits(&s->pb, 1, 0); 

    

    encode_mb_skip_run(s, 0);

    

    

    put_bits(&s->pb, 3, 1); 

    

    

    put_bits(&s->pb, 1, 1); 

    put_bits(&s->pb, 1, 1); 



    

    encode_mb_skip_run(s, s->mb_width * s->mb_height - 2);

    

    

    put_bits(&s->pb, 3, 1); 

    

    

    put_bits(&s->pb, 1, 1); 

    put_bits(&s->pb, 1, 1); 

}