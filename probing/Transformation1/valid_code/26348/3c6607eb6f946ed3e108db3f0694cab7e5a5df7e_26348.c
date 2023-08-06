int attribute_align_arg avcodec_encode_audio2(AVCodecContext *avctx,

                                              AVPacket *avpkt,

                                              const AVFrame *frame,

                                              int *got_packet_ptr)

{

    int ret;

    int user_packet = !!avpkt->data;

    int nb_samples;



    *got_packet_ptr = 0;



    if (!(avctx->codec->capabilities & CODEC_CAP_DELAY) && !frame) {

        av_free_packet(avpkt);

        av_init_packet(avpkt);

        avpkt->size = 0;

        return 0;

    }



    

    if (frame) {

        nb_samples = frame->nb_samples;

        if (avctx->codec->capabilities & CODEC_CAP_SMALL_LAST_FRAME) {

            if (nb_samples > avctx->frame_size)

                return AVERROR(EINVAL);

        } else if (!(avctx->codec->capabilities & CODEC_CAP_VARIABLE_FRAME_SIZE)) {

            if (nb_samples != avctx->frame_size)

                return AVERROR(EINVAL);

        }

    } else {

        nb_samples = avctx->frame_size;

    }



    if (avctx->codec->encode2) {

        ret = avctx->codec->encode2(avctx, avpkt, frame, got_packet_ptr);

        if (!ret && *got_packet_ptr) {

            if (!(avctx->codec->capabilities & CODEC_CAP_DELAY)) {

                if (avpkt->pts == AV_NOPTS_VALUE)

                    avpkt->pts = frame->pts;

                if (!avpkt->duration)

                    avpkt->duration = ff_samples_to_time_base(avctx,

                                                              frame->nb_samples);

            }

            avpkt->dts = avpkt->pts;

        } else {

            avpkt->size = 0;

        }

    } else {

        

        int fs_tmp   = 0;

        int buf_size = avpkt->size;

        if (!user_packet) {

            if (avctx->codec->capabilities & CODEC_CAP_VARIABLE_FRAME_SIZE) {

                av_assert0(av_get_bits_per_sample(avctx->codec_id) != 0);

                buf_size = nb_samples * avctx->channels *

                           av_get_bits_per_sample(avctx->codec_id) / 8;

            } else {

                

                buf_size = 2 * avctx->frame_size * avctx->channels *

                           av_get_bytes_per_sample(avctx->sample_fmt);

                buf_size += FF_MIN_BUFFER_SIZE;

            }

        }

        if ((ret = ff_alloc_packet(avpkt, buf_size)))

            return ret;



        

        if ((avctx->codec->capabilities & CODEC_CAP_SMALL_LAST_FRAME) &&

            nb_samples < avctx->frame_size) {

            fs_tmp = avctx->frame_size;

            avctx->frame_size = nb_samples;

        }



        

        ret = avctx->codec->encode(avctx, avpkt->data, avpkt->size,

                                   frame ? frame->data[0] : NULL);

        if (ret >= 0) {

            if (!ret) {

                

                if (!user_packet)

                    av_freep(&avpkt->data);

            } else {

                if (avctx->coded_frame)

                    avpkt->pts = avpkt->dts = avctx->coded_frame->pts;

                

                if (fs_tmp) {

                    avpkt->duration = ff_samples_to_time_base(avctx,

                                                              avctx->frame_size);

                }

            }

            avpkt->size = ret;

            *got_packet_ptr = (ret > 0);

            ret = 0;

        }



        if (fs_tmp)

            avctx->frame_size = fs_tmp;

    }

    if (!ret) {

        if (!user_packet && avpkt->data) {

            uint8_t *new_data = av_realloc(avpkt->data, avpkt->size);

            if (new_data)

                avpkt->data = new_data;

        }



        avctx->frame_number++;

    }



    if (ret < 0 || !*got_packet_ptr)

        av_free_packet(avpkt);



    

    avpkt->flags |= AV_PKT_FLAG_KEY;



    return ret;

}