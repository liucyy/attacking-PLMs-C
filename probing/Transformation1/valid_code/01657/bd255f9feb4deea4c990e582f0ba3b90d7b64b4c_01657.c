int attribute_align_arg avcodec_decode_video2(AVCodecContext *avctx, AVFrame *picture,

                                              int *got_picture_ptr,

                                              AVPacket *avpkt)

{

    int ret;



    *got_picture_ptr = 0;

    if ((avctx->coded_width || avctx->coded_height) && av_image_check_size(avctx->coded_width, avctx->coded_height, 0, avctx))

        return -1;



    avctx->pkt = avpkt;

    apply_param_change(avctx, avpkt);



    avcodec_get_frame_defaults(picture);



    if ((avctx->codec->capabilities & CODEC_CAP_DELAY) || avpkt->size || (avctx->active_thread_type & FF_THREAD_FRAME)) {

        if (HAVE_THREADS && avctx->active_thread_type & FF_THREAD_FRAME)

            ret = ff_thread_decode_frame(avctx, picture, got_picture_ptr,

                                         avpkt);

        else {

            ret = avctx->codec->decode(avctx, picture, got_picture_ptr,

                                       avpkt);

            picture->pkt_dts             = avpkt->dts;

            picture->sample_aspect_ratio = avctx->sample_aspect_ratio;

            picture->width               = avctx->width;

            picture->height              = avctx->height;

            picture->format              = avctx->pix_fmt;

        }



        emms_c(); 



        if (*got_picture_ptr)

            avctx->frame_number++;

    } else

        ret = 0;



    

    picture->extended_data = picture->data;



    return ret;

}