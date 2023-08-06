void mjpeg_picture_header(MpegEncContext *s)

{

    put_marker(&s->pb, SOI);



    jpeg_table_header(s);



    put_marker(&s->pb, SOF0);



    put_bits(&s->pb, 16, 17);

    put_bits(&s->pb, 8, 8); 

    put_bits(&s->pb, 16, s->height);

    put_bits(&s->pb, 16, s->width);

    put_bits(&s->pb, 8, 3); 

    

    

    put_bits(&s->pb, 8, 1); 

    put_bits(&s->pb, 4, 2); 

    put_bits(&s->pb, 4, 2); 

    put_bits(&s->pb, 8, 0); 

    

    

    put_bits(&s->pb, 8, 2); 

    put_bits(&s->pb, 4, 1); 

    put_bits(&s->pb, 4, 1); 

    put_bits(&s->pb, 8, 0); 



    

    put_bits(&s->pb, 8, 3); 

    put_bits(&s->pb, 4, 1); 

    put_bits(&s->pb, 4, 1); 

    put_bits(&s->pb, 8, 0); 



    

    put_marker(&s->pb, SOS);

    put_bits(&s->pb, 16, 12); 

    put_bits(&s->pb, 8, 3); 

    

    

    put_bits(&s->pb, 8, 1); 

    put_bits(&s->pb, 4, 0); 

    put_bits(&s->pb, 4, 0); 

    

    

    put_bits(&s->pb, 8, 2); 

    put_bits(&s->pb, 4, 1); 

    put_bits(&s->pb, 4, 1); 

    

    

    put_bits(&s->pb, 8, 3); 

    put_bits(&s->pb, 4, 1); 

    put_bits(&s->pb, 4, 1); 



    put_bits(&s->pb, 8, 0); 

    put_bits(&s->pb, 8, 63); 

    put_bits(&s->pb, 8, 0); 

}