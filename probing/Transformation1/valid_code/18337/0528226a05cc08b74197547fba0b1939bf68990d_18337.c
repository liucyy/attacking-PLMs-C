static int a64_write_header(AVFormatContext *s)

{

    AVCodecContext *avctx = s->streams[0]->codec;

    uint8_t header[5] = {

        0x00, 

        0x40, 

        0x00, 

        0x00, 

        0x00  

    };



    if (avctx->extradata_size < 4) {

        av_log(s, AV_LOG_ERROR, "Missing extradata\n");

        return AVERROR(EINVAL);

    }



    switch (avctx->codec->id) {

    case AV_CODEC_ID_A64_MULTI:

        header[2] = 0x00;

        header[3] = AV_RB32(avctx->extradata+0);

        header[4] = 2;

        break;

    case AV_CODEC_ID_A64_MULTI5:

        header[2] = 0x01;

        header[3] = AV_RB32(avctx->extradata+0);

        header[4] = 3;

        break;

    default:

        return AVERROR(EINVAL);

    }

    avio_write(s->pb, header, 2);

    return 0;

}