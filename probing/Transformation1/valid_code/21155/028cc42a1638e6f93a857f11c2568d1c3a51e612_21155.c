static int read_gab2_sub(AVStream *st, AVPacket *pkt) {

    if (!strcmp(pkt->data, "GAB2") && AV_RL16(pkt->data+5) == 2) {

        uint8_t desc[256];

        int score = AVPROBE_SCORE_MAX / 2, ret;

        AVIStream *ast = st->priv_data;

        AVInputFormat *sub_demuxer;

        AVRational time_base;

        AVIOContext *pb = avio_alloc_context( pkt->data + 7,

                                              pkt->size - 7,

                                              0, NULL, NULL, NULL, NULL);

        AVProbeData pd;

        unsigned int desc_len = avio_rl32(pb);



        if (desc_len > pb->buf_end - pb->buf_ptr)

            goto error;



        ret = avio_get_str16le(pb, desc_len, desc, sizeof(desc));

        avio_skip(pb, desc_len - ret);

        if (*desc)

            av_dict_set(&st->metadata, "title", desc, 0);



        avio_rl16(pb);   

        avio_rl32(pb);   



        pd = (AVProbeData) { .buf = pb->buf_ptr, .buf_size = pb->buf_end - pb->buf_ptr };

        if (!(sub_demuxer = av_probe_input_format2(&pd, 1, &score)))

            goto error;



        if (!(ast->sub_ctx = avformat_alloc_context()))

            goto error;



        ast->sub_ctx->pb      = pb;

        if (!avformat_open_input(&ast->sub_ctx, "", sub_demuxer, NULL)) {

            ff_read_packet(ast->sub_ctx, &ast->sub_pkt);

            *st->codec = *ast->sub_ctx->streams[0]->codec;

            ast->sub_ctx->streams[0]->codec->extradata = NULL;

            time_base = ast->sub_ctx->streams[0]->time_base;

            avpriv_set_pts_info(st, 64, time_base.num, time_base.den);

        }

        ast->sub_buffer = pkt->data;

        memset(pkt, 0, sizeof(*pkt));

        return 1;

error:

        av_freep(&pb);

    }

    return 0;

}