static int handle_packet(MpegTSContext *ts, const uint8_t *packet)

{

    AVFormatContext *s = ts->stream;

    MpegTSFilter *tss;

    int len, pid, cc, cc_ok, afc, is_start;

    const uint8_t *p, *p_end;

    int64_t pos;



    pid = AV_RB16(packet + 1) & 0x1fff;

    if(pid && discard_pid(ts, pid))

        return 0;

    is_start = packet[1] & 0x40;

    tss = ts->pids[pid];

    if (ts->auto_guess && tss == NULL && is_start) {

        add_pes_stream(ts, pid, -1, 0);

        tss = ts->pids[pid];

    }

    if (!tss)

        return 0;



    

    cc = (packet[3] & 0xf);

    cc_ok = (tss->last_cc < 0) || ((((tss->last_cc + 1) & 0x0f) == cc));

    tss->last_cc = cc;



    

    afc = (packet[3] >> 4) & 3;

    p = packet + 4;

    if (afc == 0) 

        return 0;

    if (afc == 2) 

        return 0;

    if (afc == 3) {

        

        p += p[0] + 1;

    }

    

    p_end = packet + TS_PACKET_SIZE;

    if (p >= p_end)

        return 0;



    pos = url_ftell(ts->stream->pb);

    ts->pos47= pos % ts->raw_packet_size;



    if (tss->type == MPEGTS_SECTION) {

        if (is_start) {

            

            len = *p++;

            if (p + len > p_end)

                return 0;

            if (len && cc_ok) {

                

                write_section_data(s, tss,

                                   p, len, 0);

                

                if (!ts->pids[pid])

                    return 0;

            }

            p += len;

            if (p < p_end) {

                write_section_data(s, tss,

                                   p, p_end - p, 1);

            }

        } else {

            if (cc_ok) {

                write_section_data(s, tss,

                                   p, p_end - p, 0);

            }

        }

    } else {

        int ret;

        

        if ((ret = tss->u.pes_filter.pes_cb(tss, p, p_end - p, is_start,

                                            pos - ts->raw_packet_size)) < 0)

            return ret;

    }



    return 0;

}