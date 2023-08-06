static int vp9_handle_packet(AVFormatContext *ctx, PayloadContext *rtp_vp9_ctx,

                             AVStream *st, AVPacket *pkt, uint32_t *timestamp,

                             const uint8_t *buf, int len, uint16_t seq,

                             int flags)

{

    int has_pic_id, has_layer_idc, has_ref_idc, has_ss_data, has_su_data;

    av_unused int pic_id = 0, non_key_frame = 0;

    av_unused int layer_temporal = -1, layer_spatial = -1, layer_quality = -1;

    int ref_fields = 0, has_ref_field_ext_pic_id = 0;

    int first_fragment, last_fragment;

    int rtp_m;

    int res = 0;



    

    if (rtp_vp9_ctx->buf && rtp_vp9_ctx->timestamp != *timestamp)

        ffio_free_dyn_buf(&rtp_vp9_ctx->buf);



    

    if (len < RTP_VP9_DESC_REQUIRED_SIZE + 1) {

        av_log(ctx, AV_LOG_ERROR, "Too short RTP/VP9 packet, got %d bytes\n", len);

        return AVERROR_INVALIDDATA;

    }



    

    has_pic_id     = !!(buf[0] & 0x80);

    has_layer_idc  = !!(buf[0] & 0x40);

    has_ref_idc    = !!(buf[0] & 0x20);

    first_fragment = !!(buf[0] & 0x10);

    last_fragment  = !!(buf[0] & 0x08);

    has_ss_data    = !!(buf[0] & 0x04);

    has_su_data    = !!(buf[0] & 0x02);



    rtp_m = !!(flags & RTP_FLAG_MARKER);



    

    if (last_fragment != rtp_m) {

        av_log(ctx, AV_LOG_ERROR, "Invalid combination of B and M marker (%d != %d)\n", last_fragment, rtp_m);

        return AVERROR_INVALIDDATA;

    }



    

    buf += RTP_VP9_DESC_REQUIRED_SIZE;

    len -= RTP_VP9_DESC_REQUIRED_SIZE;



    

    if (has_pic_id) {

        if (len < 1) {

            av_log(ctx, AV_LOG_ERROR, "Too short RTP/VP9 packet\n");

            return AVERROR_INVALIDDATA;

        }



        

        if (buf[0] & 0x80) {

            if (len < 2) {

                av_log(ctx, AV_LOG_ERROR, "Too short RTP/VP9 packet\n");

                return AVERROR_INVALIDDATA;

            }

            pic_id = AV_RB16(buf) & 0x7fff;

            buf += 2;

            len -= 2;

        } else {

            pic_id = buf[0] & 0x7f;

            buf++;

            len--;

        }

    }



    

    if (has_layer_idc) {

        if (len < 1) {

            av_log(ctx, AV_LOG_ERROR, "Too short RTP/VP9 packet\n");

            return AVERROR_INVALIDDATA;

        }

        layer_temporal = buf[0] & 0xC0;

        layer_spatial  = buf[0] & 0x30;

        layer_quality  = buf[0] & 0x0C;

        if (has_ref_idc) {

            ref_fields = buf[0] & 0x03;

            if (ref_fields)

                non_key_frame = 1;

        }

        buf++;

        len--;

    }



    

    if (has_ref_idc) {

        while (ref_fields) {

            if (len < 1) {

                av_log(ctx, AV_LOG_ERROR, "Too short RTP/VP9 packet\n");

                return AVERROR_INVALIDDATA;

            }



            has_ref_field_ext_pic_id = buf[0] & 0x10;



            

            if (has_ref_field_ext_pic_id) {

                if (len < 2) {

                    av_log(ctx, AV_LOG_ERROR, "Too short RTP/VP9 packet\n");

                    return AVERROR_INVALIDDATA;

                }



                



                buf += 2;

                len -= 2;

            } else {



                



                buf++;

                len--;

            }

            ref_fields--;

        }

    }



    

    if (has_ss_data) {

        avpriv_report_missing_feature(ctx, "VP9 scalability structure data");

        return AVERROR(ENOSYS);

    }



    

    if (has_su_data) {

        avpriv_report_missing_feature(ctx, "VP9 scalability update structure data");

        return AVERROR(ENOSYS);

    }



    

    



    

    if (len < 1) {

        av_log(ctx, AV_LOG_ERROR, "Too short RTP/VP9 packet\n");

        return AVERROR_INVALIDDATA;

    }



    

    if (!rtp_vp9_ctx->buf) {

        

        if (first_fragment) {

            res = avio_open_dyn_buf(&rtp_vp9_ctx->buf);

            if (res < 0)

                return res;

            

            rtp_vp9_ctx->timestamp = *timestamp;

        } else {

            

            return AVERROR(EAGAIN);

        }

    }



    

    avio_write(rtp_vp9_ctx->buf, buf, len);



    

    if (!last_fragment)

        return AVERROR(EAGAIN);



    

    res = ff_rtp_finalize_packet(pkt, &rtp_vp9_ctx->buf, st->index);

    if (res < 0)

        return res;



    return 0;

}