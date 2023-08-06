static int xa_read_packet(AVFormatContext *s,

                          AVPacket *pkt)

{

    MaxisXADemuxContext *xa = s->priv_data;

    AVStream *st = s->streams[0];

    ByteIOContext *pb = s->pb;

    unsigned int packet_size;

    int ret;



    if(xa->sent_bytes > xa->out_size)

        return AVERROR(EIO);

    

    packet_size = 15*st->codec->channels;



    ret = av_get_packet(pb, pkt, packet_size);

    if(ret != packet_size)

        return AVERROR(EIO);



    pkt->stream_index = st->index;

    xa->sent_bytes += packet_size;

    pkt->pts = xa->audio_frame_counter;

    

    xa->audio_frame_counter += 28 * st->codec->channels;



    return ret;

}