static av_cold int truemotion1_decode_init(AVCodecContext *avctx)

{

    TrueMotion1Context *s = avctx->priv_data;



    s->avctx = avctx;



    











    s->frame.data[0] = NULL;



    

    av_fast_malloc(&s->vert_pred, &s->vert_pred_size, s->avctx->width * sizeof(unsigned int));



    return 0;

}