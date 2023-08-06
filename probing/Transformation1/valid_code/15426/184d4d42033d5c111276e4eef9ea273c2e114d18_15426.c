static int colo_packet_compare_tcp(Packet *spkt, Packet *ppkt)

{

    struct tcphdr *ptcp, *stcp;

    int res;



    trace_colo_compare_main("compare tcp");



    ptcp = (struct tcphdr *)ppkt->transport_header;

    stcp = (struct tcphdr *)spkt->transport_header;



    

    if (ntohs(ppkt->ip->ip_off) & IP_DF) {

        spkt->ip->ip_id = ppkt->ip->ip_id;

        

        spkt->ip->ip_sum = ppkt->ip->ip_sum;

    }



    if (ptcp->th_sum == stcp->th_sum) {

        res = colo_packet_compare_common(ppkt, spkt, ETH_HLEN);

    } else {

        res = -1;

    }



    if (res != 0 && trace_event_get_state(TRACE_COLO_COMPARE_MISCOMPARE)) {

        trace_colo_compare_pkt_info_src(inet_ntoa(ppkt->ip->ip_src),

                                        ntohl(stcp->th_seq),

                                        ntohl(stcp->th_ack),

                                        res, stcp->th_flags,

                                        spkt->size);



        trace_colo_compare_pkt_info_dst(inet_ntoa(ppkt->ip->ip_dst),

                                        ntohl(ptcp->th_seq),

                                        ntohl(ptcp->th_ack),

                                        res, ptcp->th_flags,

                                        ppkt->size);



        qemu_hexdump((char *)ppkt->data, stderr,

                     "colo-compare ppkt", ppkt->size);

        qemu_hexdump((char *)spkt->data, stderr,

                     "colo-compare spkt", spkt->size);

    }



    return res;

}