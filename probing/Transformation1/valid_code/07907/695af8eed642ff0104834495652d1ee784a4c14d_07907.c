static int field_end(H264Context *h, int in_setup)

{

    MpegEncContext *const s     = &h->s;

    AVCodecContext *const avctx = s->avctx;

    int err = 0;

    s->mb_y = 0;



    if (!in_setup && !s->droppable)

        ff_thread_report_progress(&s->current_picture_ptr->f, INT_MAX,

                                  s->picture_structure == PICT_BOTTOM_FIELD);



    if (CONFIG_H264_VDPAU_DECODER &&

        s->avctx->codec->capabilities & CODEC_CAP_HWACCEL_VDPAU)

        ff_vdpau_h264_set_reference_frames(s);



    if (in_setup || !(avctx->active_thread_type & FF_THREAD_FRAME)) {

        if (!s->droppable) {

            err = ff_h264_execute_ref_pic_marking(h, h->mmco, h->mmco_index);

            h->prev_poc_msb = h->poc_msb;

            h->prev_poc_lsb = h->poc_lsb;

        }

        h->prev_frame_num_offset = h->frame_num_offset;

        h->prev_frame_num        = h->frame_num;

        h->outputed_poc          = h->next_outputed_poc;

    }



    if (avctx->hwaccel) {

        if (avctx->hwaccel->end_frame(avctx) < 0)

            av_log(avctx, AV_LOG_ERROR,

                   "hardware accelerator failed to decode picture\n");

    }



    if (CONFIG_H264_VDPAU_DECODER &&

        s->avctx->codec->capabilities & CODEC_CAP_HWACCEL_VDPAU)

        ff_vdpau_h264_picture_complete(s);



    

    if (!FIELD_PICTURE && h->current_slice)

        ff_er_frame_end(s);



    ff_MPV_frame_end(s);



    h->current_slice = 0;



    return err;

}