void ff_mpv_frame_end(MpegEncContext *s)

{

#if FF_API_XVMC

FF_DISABLE_DEPRECATION_WARNINGS

    

    

    if (CONFIG_MPEG_XVMC_DECODER && s->avctx->xvmc_acceleration) {

        ff_xvmc_field_end(s);

    } else

FF_ENABLE_DEPRECATION_WARNINGS

#endif 



    emms_c();



    if (s->current_picture.reference)

        ff_thread_report_progress(&s->current_picture_ptr->tf, INT_MAX, 0);

}