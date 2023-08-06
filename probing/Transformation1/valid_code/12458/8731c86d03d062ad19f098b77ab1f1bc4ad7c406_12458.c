static int a64_write_trailer(struct AVFormatContext *s)

{

    A64MuxerContext *c = s->priv_data;

    AVPacket pkt;

    

    if(c->interleaved) a64_write_packet(s, &pkt);

    return 0;

}