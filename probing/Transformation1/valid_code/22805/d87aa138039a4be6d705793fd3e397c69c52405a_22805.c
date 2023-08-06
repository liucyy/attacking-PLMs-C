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



    

    if (ptcp->th_off > 5) {

        ptrdiff_t tcp_offset;



        tcp_offset = ppkt->transport_header - (uint8_t *)ppkt->data

                     + (ptcp->th_off * 4) - ppkt->vnet_hdr_len;

        res = colo_packet_compare_common(ppkt, spkt, tcp_offset);

    } else if (ptcp->th_sum == stcp->th_sum) {

        res = colo_packet_compare_common(ppkt, spkt, ETH_HLEN);

    } else {

        res = -1;

    }



    if (res != 0 && trace_event_get_state(TRACE_COLO_COMPARE_MISCOMPARE)) {

        char pri_ip_src[20], pri_ip_dst[20], sec_ip_src[20], sec_ip_dst[20];



        strcpy(pri_ip_src, inet_ntoa(ppkt->ip->ip_src));

        strcpy(pri_ip_dst, inet_ntoa(ppkt->ip->ip_dst));

        strcpy(sec_ip_src, inet_ntoa(spkt->ip->ip_src));

        strcpy(sec_ip_dst, inet_ntoa(spkt->ip->ip_dst));



        trace_colo_compare_ip_info(ppkt->size, pri_ip_src,

                                   pri_ip_dst, spkt->size,

                                   sec_ip_src, sec_ip_dst);



        trace_colo_compare_tcp_info("pri tcp packet",

                                    ntohl(ptcp->th_seq),

                                    ntohl(ptcp->th_ack),

                                    res, ptcp->th_flags,

                                    ppkt->size);



        trace_colo_compare_tcp_info("sec tcp packet",

                                    ntohl(stcp->th_seq),

                                    ntohl(stcp->th_ack),

                                    res, stcp->th_flags,

                                    spkt->size);



        qemu_hexdump((char *)ppkt->data, stderr,

                     "colo-compare ppkt", ppkt->size);

        qemu_hexdump((char *)spkt->data, stderr,

                     "colo-compare spkt", spkt->size);

    }



    return res;

}