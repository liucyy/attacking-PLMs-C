void rtp_parse_close(RTPDemuxContext *s)

{

    



    if (!strcmp(ff_rtp_enc_name(s->payload_type), "MP2T")) {

        ff_mpegts_parse_close(s->ts);

    }

    av_free(s);

}