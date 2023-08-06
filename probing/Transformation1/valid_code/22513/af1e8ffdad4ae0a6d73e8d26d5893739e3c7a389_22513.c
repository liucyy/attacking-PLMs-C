static int spdif_write_packet(struct AVFormatContext *s, AVPacket *pkt)

{

    IEC61937Context *ctx = s->priv_data;

    int ret, padding;



    ctx->out_buf = pkt->data;

    ctx->out_bytes = pkt->size;

    ctx->length_code = FFALIGN(pkt->size, 2) << 3;

    ctx->use_preamble = 1;

    ctx->extra_bswap = 0;



    ret = ctx->header_info(s, pkt);

    if (ret < 0)

        return ret;

    if (!ctx->pkt_offset)

        return 0;



    padding = (ctx->pkt_offset - ctx->use_preamble * BURST_HEADER_SIZE - ctx->out_bytes) & ~1;

    if (padding < 0) {

        av_log(s, AV_LOG_ERROR, "bitrate is too high\n");

        return AVERROR(EINVAL);

    }



    if (ctx->use_preamble) {

        put_le16(s->pb, SYNCWORD1);       

        put_le16(s->pb, SYNCWORD2);       

        put_le16(s->pb, ctx->data_type);  

        put_le16(s->pb, ctx->length_code);

    }



    if (HAVE_BIGENDIAN ^ ctx->extra_bswap) {

    put_buffer(s->pb, ctx->out_buf, ctx->out_bytes & ~1);

    } else {

    av_fast_malloc(&ctx->buffer, &ctx->buffer_size, ctx->out_bytes + FF_INPUT_BUFFER_PADDING_SIZE);

    if (!ctx->buffer)

        return AVERROR(ENOMEM);

    ff_spdif_bswap_buf16((uint16_t *)ctx->buffer, (uint16_t *)ctx->out_buf, ctx->out_bytes >> 1);

    put_buffer(s->pb, ctx->buffer, ctx->out_bytes & ~1);

    }



    if (ctx->out_bytes & 1)

        put_be16(s->pb, ctx->out_buf[ctx->out_bytes - 1]);



    put_nbyte(s->pb, 0, padding);



    av_log(s, AV_LOG_DEBUG, "type=%x len=%i pkt_offset=%i\n",

           ctx->data_type, ctx->out_bytes, ctx->pkt_offset);



    put_flush_packet(s->pb);

    return 0;

}