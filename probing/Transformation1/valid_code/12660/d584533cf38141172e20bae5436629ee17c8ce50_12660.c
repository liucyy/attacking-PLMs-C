static AVPacket *add_to_pktbuf(AVPacketList **packet_buffer, AVPacket *pkt,

                               AVPacketList **plast_pktl)

{

    AVPacketList *pktl = av_mallocz(sizeof(AVPacketList));

    if (!pktl)

        return NULL;



    if (*packet_buffer)

        (*plast_pktl)->next = pktl;

    else

        *packet_buffer = pktl;



    

    *plast_pktl = pktl;

    pktl->pkt   = *pkt;

    return &pktl->pkt;

}