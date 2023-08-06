static av_cold int pcm_dvd_decode_init(AVCodecContext *avctx)

{

    PCMDVDContext *s = avctx->priv_data;



    

    s->last_header = -1;

    

    if (!(s->extra_samples = av_malloc(8 * 3 * 4)))

        return AVERROR(ENOMEM);

    s->extra_sample_count = 0;



    return 0;

}