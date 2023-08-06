static int jpeg_parse_packet(AVFormatContext *ctx, PayloadContext *jpeg,

                             AVStream *st, AVPacket *pkt, uint32_t *timestamp,

                             const uint8_t *buf, int len, uint16_t seq,

                             int flags)

{

    uint8_t type, q, width, height;

    const uint8_t *qtables = NULL;

    uint16_t qtable_len;

    uint32_t off;

    int ret;



    if (len < 8) {

        av_log(ctx, AV_LOG_ERROR, "Too short RTP/JPEG packet.\n");

        return AVERROR_INVALIDDATA;

    }



    

    off    = AV_RB24(buf + 1);  

    type   = AV_RB8(buf + 4);   

    q      = AV_RB8(buf + 5);   

    width  = AV_RB8(buf + 6);   

    height = AV_RB8(buf + 7);   

    buf += 8;

    len -= 8;



    

    if (type > 63) {

        av_log(ctx, AV_LOG_ERROR,

               "Unimplemented RTP/JPEG restart marker header.\n");

        return AVERROR_PATCHWELCOME;

    }

    if (type > 1) {

        av_log(ctx, AV_LOG_ERROR, "Unimplemented RTP/JPEG type %d\n", type);

        return AVERROR_PATCHWELCOME;

    }



    

    if (off == 0) {

        

        uint8_t new_qtables[128];

        uint8_t hdr[1024];



        if (q > 127) {

            uint8_t precision;

            if (len < 4) {

                av_log(ctx, AV_LOG_ERROR, "Too short RTP/JPEG packet.\n");

                return AVERROR_INVALIDDATA;

            }



            

            precision  = AV_RB8(buf + 1);    

            qtable_len = AV_RB16(buf + 2);   

            buf += 4;

            len -= 4;



            if (precision)

                av_log(ctx, AV_LOG_WARNING, "Only 8-bit precision is supported.\n");



            if (qtable_len > 0) {

                if (len < qtable_len) {

                    av_log(ctx, AV_LOG_ERROR, "Too short RTP/JPEG packet.\n");

                    return AVERROR_INVALIDDATA;

                }

                qtables = buf;

                buf += qtable_len;

                len -= qtable_len;

                if (q < 255) {

                    if (jpeg->qtables_len[q - 128] &&

                        (jpeg->qtables_len[q - 128] != qtable_len ||

                         memcmp(qtables, &jpeg->qtables[q - 128][0], qtable_len))) {

                        av_log(ctx, AV_LOG_WARNING,

                               "Quantization tables for q=%d changed\n", q);

                    } else if (!jpeg->qtables_len[q - 128] && qtable_len <= 128) {

                        memcpy(&jpeg->qtables[q - 128][0], qtables,

                               qtable_len);

                        jpeg->qtables_len[q - 128] = qtable_len;

                    }

                }

            } else {

                if (q == 255) {

                    av_log(ctx, AV_LOG_ERROR,

                           "Invalid RTP/JPEG packet. Quantization tables not found.\n");

                    return AVERROR_INVALIDDATA;

                }

                if (!jpeg->qtables_len[q - 128]) {

                    av_log(ctx, AV_LOG_ERROR,

                           "No quantization tables known for q=%d yet.\n", q);

                    return AVERROR_INVALIDDATA;

                }

                qtables    = &jpeg->qtables[q - 128][0];

                qtable_len =  jpeg->qtables_len[q - 128];

            }

        } else { 

            if (q == 0 || q > 99) {

                av_log(ctx, AV_LOG_ERROR, "Reserved q value %d\n", q);

                return AVERROR_INVALIDDATA;

            }

            create_default_qtables(new_qtables, q);

            qtables    = new_qtables;

            qtable_len = sizeof(new_qtables);

        }



        

        ffio_free_dyn_buf(&jpeg->frame);



        if ((ret = avio_open_dyn_buf(&jpeg->frame)) < 0)

            return ret;

        jpeg->timestamp = *timestamp;



        

        jpeg->hdr_size = jpeg_create_header(hdr, sizeof(hdr), type, width,

                                            height, qtables,

                                            qtable_len / 64);



        

        avio_write(jpeg->frame, hdr, jpeg->hdr_size);

    }



    if (!jpeg->frame) {

        av_log(ctx, AV_LOG_ERROR,

               "Received packet without a start chunk; dropping frame.\n");

        return AVERROR(EAGAIN);

    }



    if (jpeg->timestamp != *timestamp) {

        

        ffio_free_dyn_buf(&jpeg->frame);

        av_log(ctx, AV_LOG_ERROR, "RTP timestamps don't match.\n");

        return AVERROR_INVALIDDATA;

    }



    if (off != avio_tell(jpeg->frame) - jpeg->hdr_size) {

        av_log(ctx, AV_LOG_ERROR,

               "Missing packets; dropping frame.\n");

        return AVERROR(EAGAIN);

    }



    

    avio_write(jpeg->frame, buf, len);



    if (flags & RTP_FLAG_MARKER) {

        

        uint8_t buf[2] = { 0xff, EOI };



        

        avio_write(jpeg->frame, buf, sizeof(buf));



        

        if ((ret = ff_rtp_finalize_packet(pkt, &jpeg->frame, st->index)) < 0) {

            av_log(ctx, AV_LOG_ERROR,

                   "Error occurred when getting frame buffer.\n");

            return ret;

        }



        return 0;

    }



    return AVERROR(EAGAIN);

}