int rtp_check_and_send_back_rr(RTPDemuxContext *s, int count)

{

    ByteIOContext pb;

    uint8_t *buf;

    int len;

    int rtcp_bytes;



    if (!s->rtp_ctx || (count < 1))

        return -1;



    

    s->octet_count += count;

    rtcp_bytes = ((s->octet_count - s->last_octet_count) * RTCP_TX_RATIO_NUM) /

        RTCP_TX_RATIO_DEN;

    rtcp_bytes /= 50; 

    if (rtcp_bytes < 28)

        return -1;

    s->last_octet_count = s->octet_count;



    if (url_open_dyn_buf(&pb) < 0)

        return -1;



    

    put_byte(&pb, (RTP_VERSION << 6) + 1); 

    put_byte(&pb, 201);

    put_be16(&pb, 7); 

    put_be32(&pb, s->ssrc); 

    put_be32(&pb, s->ssrc); 

    

    put_be32(&pb, ((0 << 24) | (0 & 0x0ffffff))); 

    put_be32(&pb, (0 << 16) | s->seq);

    put_be32(&pb, 0x68); 

    put_be32(&pb, -1); 

    put_be32(&pb, 1); 



    

    put_byte(&pb, (RTP_VERSION << 6) + 1); 

    put_byte(&pb, 202);

    len = strlen(s->hostname);

    put_be16(&pb, (6 + len + 3) / 4); 

    put_be32(&pb, s->ssrc);

    put_byte(&pb, 0x01);

    put_byte(&pb, len);

    put_buffer(&pb, s->hostname, len);

    

    for (len = (6 + len) % 4; len % 4; len++) {

        put_byte(&pb, 0);

    }



    put_flush_packet(&pb);

    len = url_close_dyn_buf(&pb, &buf);

    if ((len > 0) && buf) {

#if defined(DEBUG)

        printf("sending %d bytes of RR\n", len);

#endif

        url_write(s->rtp_ctx, buf, len);

        av_free(buf);

    }

    return 0;

}