static int hevc_handle_packet(AVFormatContext *ctx, PayloadContext *rtp_hevc_ctx,

                              AVStream *st, AVPacket *pkt, uint32_t *timestamp,

                              const uint8_t *buf, int len, uint16_t seq,

                              int flags)

{

    const uint8_t *rtp_pl = buf;

    int tid, lid, nal_type;

    int first_fragment, last_fragment, fu_type;

    uint8_t new_nal_header[2];

    int res = 0;



    

    if (len < RTP_HEVC_PAYLOAD_HEADER_SIZE + 1) {

        av_log(ctx, AV_LOG_ERROR, "Too short RTP/HEVC packet, got %d bytes\n", len);

        return AVERROR_INVALIDDATA;

    }



    

    nal_type =  (buf[0] >> 1) & 0x3f;

    lid  = ((buf[0] << 5) & 0x20) | ((buf[1] >> 3) & 0x1f);

    tid  =   buf[1] & 0x07;



    

    if (lid) {

        

        avpriv_report_missing_feature(ctx, "Multi-layer HEVC coding\n");

        return AVERROR_PATCHWELCOME;

    }



    

    if (!tid) {

        av_log(ctx, AV_LOG_ERROR, "Illegal temporal ID in RTP/HEVC packet\n");

        return AVERROR_INVALIDDATA;

    }



    

    if (nal_type > 50) {

        av_log(ctx, AV_LOG_ERROR, "Unsupported (HEVC) NAL type (%d)\n", nal_type);

        return AVERROR_INVALIDDATA;

    }



    switch (nal_type) {

    

    case 32:

    

    case 33:

    

    case 34:

    

    case 39:

    

    default:

        

        if (len < 1) {

            av_log(ctx, AV_LOG_ERROR,

                   "Too short RTP/HEVC packet, got %d bytes of NAL unit type %d\n",

                   len, nal_type);

            return AVERROR_INVALIDDATA;

        }



        

        if ((res = av_new_packet(pkt, sizeof(start_sequence) + len)) < 0)

            return res;

        

        memcpy(pkt->data, start_sequence, sizeof(start_sequence));

        

        memcpy(pkt->data + sizeof(start_sequence), buf, len);



        break;

    

    case 48:

        

        buf += RTP_HEVC_PAYLOAD_HEADER_SIZE;

        len -= RTP_HEVC_PAYLOAD_HEADER_SIZE;



        

        if (rtp_hevc_ctx->using_donl_field) {

            buf += RTP_HEVC_DONL_FIELD_SIZE;

            len -= RTP_HEVC_DONL_FIELD_SIZE;

        }



        res = ff_h264_handle_aggregated_packet(ctx, pkt, buf, len,

                                               rtp_hevc_ctx->using_donl_field ?

                                               RTP_HEVC_DOND_FIELD_SIZE : 0,

                                               NULL, 0);

        if (res < 0)

            return res;

        break;

    

    case 49:

        

        buf += RTP_HEVC_PAYLOAD_HEADER_SIZE;

        len -= RTP_HEVC_PAYLOAD_HEADER_SIZE;



        

        first_fragment = buf[0] & 0x80;

        last_fragment  = buf[0] & 0x40;

        fu_type        = buf[0] & 0x3f;



        

        buf += RTP_HEVC_FU_HEADER_SIZE;

        len -= RTP_HEVC_FU_HEADER_SIZE;



        

        if (rtp_hevc_ctx->using_donl_field) {

            buf += RTP_HEVC_DONL_FIELD_SIZE;

            len -= RTP_HEVC_DONL_FIELD_SIZE;

        }



        av_dlog(ctx, " FU type %d with %d bytes\n", fu_type, len);



        if (len <= 0) {

            

            av_log(ctx, AV_LOG_ERROR,

                   "Too short RTP/HEVC packet, got %d bytes of NAL unit type %d\n",

                   len, nal_type);

            return AVERROR_INVALIDDATA;

        }



        if (first_fragment && last_fragment) {

            av_log(ctx, AV_LOG_ERROR, "Illegal combination of S and E bit in RTP/HEVC packet\n");

            return AVERROR_INVALIDDATA;

        }



        new_nal_header[0] = (rtp_pl[0] & 0x81) | (fu_type << 1);

        new_nal_header[1] = rtp_pl[1];



        res = ff_h264_handle_frag_packet(pkt, buf, len, first_fragment,

                                         new_nal_header, sizeof(new_nal_header));



        break;

    

    case 50:

        

        avpriv_report_missing_feature(ctx, "PACI packets for RTP/HEVC\n");

        res = AVERROR_PATCHWELCOME;

        break;

    }



    pkt->stream_index = st->index;



    return res;

}