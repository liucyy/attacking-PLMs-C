static int slice_end(AVCodecContext *avctx, AVFrame *pict)

{

    Mpeg1Context *s1  = avctx->priv_data;

    MpegEncContext *s = &s1->mpeg_enc_ctx;



    if (!s1->mpeg_enc_ctx_allocated || !s->current_picture_ptr)

        return 0;



    if (s->avctx->hwaccel) {

        if (s->avctx->hwaccel->end_frame(s->avctx) < 0)

            av_log(avctx, AV_LOG_ERROR,

                   "hardware accelerator failed to decode picture\n");

    }



#if FF_API_XVMC

FF_DISABLE_DEPRECATION_WARNINGS

    if (CONFIG_MPEG_XVMC_DECODER && s->avctx->xvmc_acceleration)

        ff_xvmc_field_end(s);

FF_ENABLE_DEPRECATION_WARNINGS

#endif 



    

    if ( !s->first_field) {

        



        ff_er_frame_end(&s->er);



        ff_MPV_frame_end(s);



        if (s->pict_type == AV_PICTURE_TYPE_B || s->low_delay) {

            int ret = av_frame_ref(pict, &s->current_picture_ptr->f);

            if (ret < 0)

                return ret;

            ff_print_debug_info(s, s->current_picture_ptr);

        } else {

            if (avctx->active_thread_type & FF_THREAD_FRAME)

                s->picture_number++;

            

            

            if (s->last_picture_ptr != NULL) {

                int ret = av_frame_ref(pict, &s->last_picture_ptr->f);

                if (ret < 0)

                    return ret;

                ff_print_debug_info(s, s->last_picture_ptr);

            }

        }



        return 1;

    } else {

        return 0;

    }

}