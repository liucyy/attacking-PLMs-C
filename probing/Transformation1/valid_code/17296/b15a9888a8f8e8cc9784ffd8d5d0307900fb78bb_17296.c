static int sol_read_packet(AVFormatContext *s,

                          AVPacket *pkt)

{

    int ret;



    if (s->pb->eof_reached)

        return AVERROR(EIO);

    ret= av_get_packet(s->pb, pkt, MAX_SIZE);



    pkt->stream_index = 0;



    

    pkt->size = ret;

    return 0;

}