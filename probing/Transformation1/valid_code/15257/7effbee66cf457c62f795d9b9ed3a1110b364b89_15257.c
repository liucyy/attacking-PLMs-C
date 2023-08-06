static int mp3_read_packet(AVFormatContext *s, AVPacket *pkt)

{

    int ret, size;

    



    size= MP3_PACKET_SIZE;



    ret= av_get_packet(s->pb, pkt, size);




    pkt->stream_index = 0;

    if (ret <= 0) {

        if(ret<0)

            return ret;

        return AVERROR_EOF;

    }



    if (ret > ID3v1_TAG_SIZE &&

        memcmp(&pkt->data[ret - ID3v1_TAG_SIZE], "TAG", 3) == 0)

        ret -= ID3v1_TAG_SIZE;



    

    pkt->size = ret;

    return ret;

}