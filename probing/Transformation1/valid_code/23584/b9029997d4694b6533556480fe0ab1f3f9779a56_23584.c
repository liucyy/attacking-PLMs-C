static int roq_decode_end(AVCodecContext *avctx)

{

    RoqContext *s = avctx->priv_data;



    

    avctx->release_buffer(avctx, &s->last_frame);



    return 0;

}