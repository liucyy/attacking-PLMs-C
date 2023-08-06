static int asf_write_packet(AVFormatContext *s, AVPacket *pkt)

{

    ASFContext *asf = s->priv_data;

    AVIOContext *pb = s->pb;

    ASFStream *stream;

    AVCodecContext *codec;

    uint32_t packet_number;

    int64_t pts;

    int start_sec;

    int flags = pkt->flags;

    int ret;

    uint64_t offset = avio_tell(pb);



    codec  = s->streams[pkt->stream_index]->codec;

    stream = &asf->streams[pkt->stream_index];



    if (codec->codec_type == AVMEDIA_TYPE_AUDIO)

        flags &= ~AV_PKT_FLAG_KEY;



    pts = (pkt->pts != AV_NOPTS_VALUE) ? pkt->pts : pkt->dts;

    av_assert0(pts != AV_NOPTS_VALUE);






    pts *= 10000;

    asf->duration = FFMAX(asf->duration, pts + pkt->duration * 10000);



    packet_number = asf->nb_packets;

    put_frame(s, stream, s->streams[pkt->stream_index],

              pkt->dts, pkt->data, pkt->size, flags);



    start_sec = (int)((PREROLL_TIME * 10000 + pts + ASF_INDEXED_INTERVAL - 1)

              / ASF_INDEXED_INTERVAL);



    

    if ((!asf->is_streamed) && (flags & AV_PKT_FLAG_KEY)) {

        uint16_t packet_count = asf->nb_packets - packet_number;

        ret = update_index(s, start_sec, packet_number, packet_count, offset);

        if (ret < 0)

            return ret;


    asf->end_sec = start_sec;



    return 0;