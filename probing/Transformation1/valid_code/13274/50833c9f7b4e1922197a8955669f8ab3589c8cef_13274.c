static void put_audio_specific_config(AVCodecContext *avctx)

{

    PutBitContext pb;

    AACEncContext *s = avctx->priv_data;



    init_put_bits(&pb, avctx->extradata, avctx->extradata_size*8);

    put_bits(&pb, 5, 2); 

    put_bits(&pb, 4, s->samplerate_index); 

    put_bits(&pb, 4, s->channels);

    

    put_bits(&pb, 1, 0); 

    put_bits(&pb, 1, 0); 

    put_bits(&pb, 1, 0); 



    

    put_bits(&pb, 11, 0x2b7); 

    put_bits(&pb, 5,  AOT_SBR);

    put_bits(&pb, 1,  0);

    flush_put_bits(&pb);

}