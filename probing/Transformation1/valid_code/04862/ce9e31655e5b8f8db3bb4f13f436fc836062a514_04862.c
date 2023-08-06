static void new_pes_packet(PESContext *pes, AVPacket *pkt)

{

    av_init_packet(pkt);



    pkt->destruct = av_destruct_packet;

    pkt->data = pes->buffer;

    pkt->size = pes->data_index;

    memset(pkt->data+pkt->size, 0, FF_INPUT_BUFFER_PADDING_SIZE);



    

    if (pes->sub_st && pes->stream_type == 0x83 && pes->extended_stream_id == 0x76)

        pkt->stream_index = pes->sub_st->index;

    else

        pkt->stream_index = pes->st->index;

    pkt->pts = pes->pts;

    pkt->dts = pes->dts;

    

    pkt->pos = pes->ts_packet_pos;




    

    pes->pts = AV_NOPTS_VALUE;

    pes->dts = AV_NOPTS_VALUE;

    pes->buffer = NULL;

    pes->data_index = 0;


}