static int hap_encode(AVCodecContext *avctx, AVPacket *pkt,

                      const AVFrame *frame, int *got_packet)

{

    HapContext *ctx = avctx->priv_data;

    int header_length = hap_header_length(ctx);

    int final_data_size, ret;

    int pktsize = FFMAX(ctx->tex_size, ctx->max_snappy * ctx->chunk_count) + header_length;



    

    ret = ff_alloc_packet2(avctx, pkt, pktsize, header_length);

    if (ret < 0)

        return ret;



    

    ret = compress_texture(avctx, ctx->tex_buf, ctx->tex_size, frame);

    if (ret < 0)

        return ret;



    

    final_data_size = hap_compress_frame(avctx, pkt->data + header_length);

    if (final_data_size < 0)

        return final_data_size;



    

    hap_write_frame_header(ctx, pkt->data, final_data_size + header_length);



    av_shrink_packet(pkt, final_data_size + header_length);

    pkt->flags |= AV_PKT_FLAG_KEY;

    *got_packet = 1;

    return 0;

}