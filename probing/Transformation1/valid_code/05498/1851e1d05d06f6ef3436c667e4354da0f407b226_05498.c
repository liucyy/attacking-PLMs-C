static int rtp_write(URLContext *h, const uint8_t *buf, int size)
{
    RTPContext *s = h->priv_data;
    int ret;
    URLContext *hd;
    if (RTP_PT_IS_RTCP(buf[1])) {
        
        hd = s->rtcp_hd;
    } else {
        
        hd = s->rtp_hd;
    }
    ret = ffurl_write(hd, buf, size);
    return ret;
}