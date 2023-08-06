static ssize_t colo_rewriter_receive_iov(NetFilterState *nf,

                                         NetClientState *sender,

                                         unsigned flags,

                                         const struct iovec *iov,

                                         int iovcnt,

                                         NetPacketSent *sent_cb)

{

    RewriterState *s = FILTER_COLO_REWRITER(nf);

    Connection *conn;

    ConnectionKey key;

    Packet *pkt;

    ssize_t size = iov_size(iov, iovcnt);

    char *buf = g_malloc0(size);



    iov_to_buf(iov, iovcnt, 0, buf, size);

    pkt = packet_new(buf, size);



    

    if (pkt && is_tcp_packet(pkt)) {



        fill_connection_key(pkt, &key);



        if (sender == nf->netdev) {

            

            reverse_connection_key(&key);

        }

        conn = connection_get(s->connection_track_table,

                              &key,

                              NULL);



        if (sender == nf->netdev) {

            

            

        } else {

            

            

        }

    }



    packet_destroy(pkt, NULL);

    pkt = NULL;

    return 0;

}