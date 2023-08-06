static int msnwc_tcp_read_packet(AVFormatContext *ctx, AVPacket *pkt)

{

    AVIOContext *pb = ctx->pb;

    uint16_t keyframe;

    uint32_t size, timestamp;



    avio_skip(pb, 1); 

    avio_skip(pb, 2);

    avio_skip(pb, 2);

    keyframe = avio_rl16(pb);

    size = avio_rl32(pb);

    avio_skip(pb, 4);

    avio_skip(pb, 4);

    timestamp = avio_rl32(pb);



    if(!size || av_get_packet(pb, pkt, size) != size)

        return -1;



    avio_skip(pb, 1); 



    pkt->pts = timestamp;

    pkt->dts = timestamp;

    pkt->stream_index = 0;



    

    if(keyframe&1)

        pkt->flags |= AV_PKT_FLAG_KEY;



    return HEADER_SIZE + size;

}