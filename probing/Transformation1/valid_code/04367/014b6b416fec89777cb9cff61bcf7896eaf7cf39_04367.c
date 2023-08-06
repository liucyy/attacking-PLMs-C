int update_dimensions(VP8Context *s, int width, int height, int is_vp7)

{

    AVCodecContext *avctx = s->avctx;

    int i, ret;



    if (width  != s->avctx->width ||

        height != s->avctx->height) {

        vp8_decode_flush_impl(s->avctx, 1);



        ret = ff_set_dimensions(s->avctx, width, height);

        if (ret < 0)

            return ret;

    }



    s->mb_width  = (s->avctx->coded_width  + 15) / 16;

    s->mb_height = (s->avctx->coded_height + 15) / 16;



    s->mb_layout = is_vp7 || avctx->active_thread_type == FF_THREAD_SLICE &&

                   FFMIN(s->num_coeff_partitions, avctx->thread_count) > 1;

    if (!s->mb_layout) { 

        s->macroblocks_base       = av_mallocz((s->mb_width + s->mb_height * 2 + 1) *

                                               sizeof(*s->macroblocks));

        s->intra4x4_pred_mode_top = av_mallocz(s->mb_width * 4);

    } else 

        s->macroblocks_base = av_mallocz((s->mb_width + 2) * (s->mb_height + 2) *

                                         sizeof(*s->macroblocks));

    s->top_nnz     = av_mallocz(s->mb_width * sizeof(*s->top_nnz));

    s->top_border  = av_mallocz((s->mb_width + 1) * sizeof(*s->top_border));

    s->thread_data = av_mallocz(MAX_THREADS * sizeof(VP8ThreadData));



    for (i = 0; i < MAX_THREADS; i++) {

        s->thread_data[i].filter_strength =

            av_mallocz(s->mb_width * sizeof(*s->thread_data[0].filter_strength));

#if HAVE_THREADS

        pthread_mutex_init(&s->thread_data[i].lock, NULL);

        pthread_cond_init(&s->thread_data[i].cond, NULL);

#endif

    }



    if (!s->macroblocks_base || !s->top_nnz || !s->top_border ||

        (!s->intra4x4_pred_mode_top && !s->mb_layout))

        return AVERROR(ENOMEM);



    s->macroblocks = s->macroblocks_base + 1;



    return 0;

}