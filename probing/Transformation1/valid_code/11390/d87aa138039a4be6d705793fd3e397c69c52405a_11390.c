static int colo_packet_compare_icmp(Packet *spkt, Packet *ppkt)

{

    int network_header_length = ppkt->ip->ip_hl * 4;



    trace_colo_compare_main("compare icmp");



    

    if (colo_packet_compare_common(ppkt, spkt,

                                   network_header_length + ETH_HLEN)) {

        trace_colo_compare_icmp_miscompare("primary pkt size",

                                           ppkt->size);

        trace_colo_compare_icmp_miscompare("Secondary pkt size",

                                           spkt->size);

        if (trace_event_get_state(TRACE_COLO_COMPARE_MISCOMPARE)) {

            qemu_hexdump((char *)ppkt->data, stderr, "colo-compare pri pkt",

                         ppkt->size);

            qemu_hexdump((char *)spkt->data, stderr, "colo-compare sec pkt",

                         spkt->size);

        }

        return -1;

    } else {

        return 0;

    }

}