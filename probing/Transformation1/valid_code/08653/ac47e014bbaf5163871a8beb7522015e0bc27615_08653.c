int ff_adts_write_frame_header(ADTSContext *ctx,

                               uint8_t *buf, int size, int pce_size)

{

    PutBitContext pb;



    init_put_bits(&pb, buf, ADTS_HEADER_SIZE);



    

    put_bits(&pb, 12, 0xfff);   

    put_bits(&pb, 1, 0);        

    put_bits(&pb, 2, 0);        

    put_bits(&pb, 1, 1);        

    put_bits(&pb, 2, ctx->objecttype); 

    put_bits(&pb, 4, ctx->sample_rate_index);

    put_bits(&pb, 1, 0);        

    put_bits(&pb, 3, ctx->channel_conf); 

    put_bits(&pb, 1, 0);        

    put_bits(&pb, 1, 0);        



    

    put_bits(&pb, 1, 0);        

    put_bits(&pb, 1, 0);        

    put_bits(&pb, 13, ADTS_HEADER_SIZE + size + pce_size); 

    put_bits(&pb, 11, 0x7ff);   

    put_bits(&pb, 2, 0);        



    flush_put_bits(&pb);



    return 0;

}