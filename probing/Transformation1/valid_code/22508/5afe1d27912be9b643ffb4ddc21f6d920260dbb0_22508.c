static int mpegts_raw_read_packet(AVFormatContext *s,

                                  AVPacket *pkt)

{

    MpegTSContext *ts = s->priv_data;

    int ret, i;

    int64_t pcr_h, next_pcr_h, pos;

    int pcr_l, next_pcr_l;

    uint8_t pcr_buf[12];

    uint8_t *data;



    if (av_new_packet(pkt, TS_PACKET_SIZE) < 0)

        return AVERROR(ENOMEM);

    pkt->pos= avio_tell(s->pb);

    ret = read_packet(s, pkt->data, ts->raw_packet_size, &data);

    if (ret < 0) {

        av_free_packet(pkt);

        return ret;

    }

    if (data != pkt->data)

        memcpy(pkt->data, data, ts->raw_packet_size);

    finished_reading_packet(s, ts->raw_packet_size);

    if (ts->mpeg2ts_compute_pcr) {

        

        if (parse_pcr(&pcr_h, &pcr_l, pkt->data) == 0) {

            

            pos = avio_tell(s->pb);

            for(i = 0; i < MAX_PACKET_READAHEAD; i++) {

                avio_seek(s->pb, pos + i * ts->raw_packet_size, SEEK_SET);

                avio_read(s->pb, pcr_buf, 12);

                if (parse_pcr(&next_pcr_h, &next_pcr_l, pcr_buf) == 0) {

                    

                    ts->pcr_incr = ((next_pcr_h - pcr_h) * 300 + (next_pcr_l - pcr_l)) /

                        (i + 1);

                    break;

                }

            }

            avio_seek(s->pb, pos, SEEK_SET);

            

            ts->cur_pcr = pcr_h * 300 + pcr_l;

        }

        pkt->pts = ts->cur_pcr;

        pkt->duration = ts->pcr_incr;

        ts->cur_pcr += ts->pcr_incr;

    }

    pkt->stream_index = 0;

    return 0;

}