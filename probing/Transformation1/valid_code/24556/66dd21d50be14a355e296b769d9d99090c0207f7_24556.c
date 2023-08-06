int attribute_align_arg avcodec_send_packet(AVCodecContext *avctx, const AVPacket *avpkt)

{

    int ret;



    if (!avcodec_is_open(avctx) || !av_codec_is_decoder(avctx->codec))

        return AVERROR(EINVAL);



    if (avctx->internal->draining)

        return AVERROR_EOF;



    if (!avpkt || !avpkt->size) {

        avctx->internal->draining = 1;

        avpkt = NULL;



        if (!(avctx->codec->capabilities & AV_CODEC_CAP_DELAY))

            return 0;

    }



    if (avctx->codec->send_packet) {

        if (avpkt) {

            ret = apply_param_change(avctx, (AVPacket *)avpkt);

            if (ret < 0)

                return ret;

        }

        return avctx->codec->send_packet(avctx, avpkt);

    }



    

    



    if (avctx->internal->buffer_pkt->size || avctx->internal->buffer_frame->buf[0])

        return AVERROR(EAGAIN);



    

    

    

    

    return do_decode(avctx, (AVPacket *)avpkt);

}