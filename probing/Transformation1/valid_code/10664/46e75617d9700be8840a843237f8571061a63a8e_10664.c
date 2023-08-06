static av_cold int truemotion1_decode_init(AVCodecContext *avctx)

{

    TrueMotion1Context *s = avctx->priv_data;



    s->avctx = avctx;



    











    s->frame = av_frame_alloc();

    if (!s->frame)

        return AVERROR(ENOMEM);



    

    av_fast_malloc(&s->vert_pred, &s->vert_pred_size, s->avctx->width * sizeof(unsigned int));

    if (!s->vert_pred)

        return AVERROR(ENOMEM);



    return 0;

}