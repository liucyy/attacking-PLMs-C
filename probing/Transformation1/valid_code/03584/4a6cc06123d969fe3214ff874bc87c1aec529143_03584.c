int rtp_parse_packet(RTPDemuxContext *s, AVPacket *pkt,

                     const uint8_t *buf, int len)

{

    unsigned int ssrc, h;

    int payload_type, seq, ret;

    AVStream *st;

    uint32_t timestamp;

    int rv= 0;



    if (!buf) {

        

        if(s->st && s->parse_packet) {

            timestamp= 0; 

            rv= s->parse_packet(s, pkt, &timestamp, NULL, 0);

            finalize_packet(s, pkt, timestamp);

            return rv;

        } else {

            

            if (s->read_buf_index >= s->read_buf_size)

                return -1;

            ret = mpegts_parse_packet(s->ts, pkt, s->buf + s->read_buf_index,

                                      s->read_buf_size - s->read_buf_index);

            if (ret < 0)

                return -1;

            s->read_buf_index += ret;

            if (s->read_buf_index < s->read_buf_size)

                return 1;

            else

                return 0;

        }

    }



    if (len < 12)

        return -1;



    if ((buf[0] & 0xc0) != (RTP_VERSION << 6))

        return -1;

    if (buf[1] >= 200 && buf[1] <= 204) {

        rtcp_parse_packet(s, buf, len);

        return -1;

    }

    payload_type = buf[1] & 0x7f;

    seq  = (buf[2] << 8) | buf[3];

    timestamp = decode_be32(buf + 4);

    ssrc = decode_be32(buf + 8);

    

    s->ssrc = ssrc;



    

    if (s->payload_type != payload_type)

        return -1;



    st = s->st;

#if defined(DEBUG) || 1

    if (seq != ((s->seq + 1) & 0xffff)) {

        av_log(st?st->codec:NULL, AV_LOG_ERROR, "RTP: PT=%02x: bad cseq %04x expected=%04x\n",

               payload_type, seq, ((s->seq + 1) & 0xffff));

    }

#endif

    s->seq = seq;

    len -= 12;

    buf += 12;



    if (!st) {

        

        ret = mpegts_parse_packet(s->ts, pkt, buf, len);

        if (ret < 0)

            return -1;

        if (ret < len) {

            s->read_buf_size = len - ret;

            memcpy(s->buf, buf + ret, s->read_buf_size);

            s->read_buf_index = 0;

            return 1;

        }

    } else {

        

        switch(st->codec->codec_id) {

        case CODEC_ID_MP2:

            

            if (len <= 4)

                return -1;

            h = decode_be32(buf);

            len -= 4;

            buf += 4;

            av_new_packet(pkt, len);

            memcpy(pkt->data, buf, len);

            break;

        case CODEC_ID_MPEG1VIDEO:

            

            if (len <= 4)

                return -1;

            h = decode_be32(buf);

            buf += 4;

            len -= 4;

            if (h & (1 << 26)) {

                

                if (len <= 4)

                    return -1;

                buf += 4;

                len -= 4;

            }

            av_new_packet(pkt, len);

            memcpy(pkt->data, buf, len);

            break;

            

            

            

        case CODEC_ID_MPEG4AAC:

            if (rtp_parse_mp4_au(s, buf))

                return -1;

            {

                rtp_payload_data_t *infos = s->rtp_payload_data;

                if (infos == NULL)

                    return -1;

                buf += infos->au_headers_length_bytes + 2;

                len -= infos->au_headers_length_bytes + 2;



                

                av_new_packet(pkt, infos->au_headers[0].size);

                memcpy(pkt->data, buf, infos->au_headers[0].size);

                buf += infos->au_headers[0].size;

                len -= infos->au_headers[0].size;

            }

            s->read_buf_size = len;

            s->buf_ptr = buf;

            rv= 0;

            break;

        default:

            if(s->parse_packet) {

                rv= s->parse_packet(s, pkt, &timestamp, buf, len);

            } else {

                av_new_packet(pkt, len);

                memcpy(pkt->data, buf, len);

            }

            break;

        }



        

        finalize_packet(s, pkt, timestamp);

    }

    return rv;

}