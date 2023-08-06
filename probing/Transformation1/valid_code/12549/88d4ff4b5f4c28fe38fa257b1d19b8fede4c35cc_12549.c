int av_read_frame(AVFormatContext *s, AVPacket *pkt)

{

    const int genpts = s->flags & AVFMT_FLAG_GENPTS;

    int          eof = 0;

    int ret;

    AVStream *st;



    if (!genpts) {

        ret = s->packet_buffer ?

            read_from_packet_buffer(&s->packet_buffer, &s->packet_buffer_end, pkt) :

            read_frame_internal(s, pkt);

        if (ret < 0)

            return ret;

        goto return_packet;

    }



    for (;;) {

        AVPacketList *pktl = s->packet_buffer;



        if (pktl) {

            AVPacket *next_pkt = &pktl->pkt;



            if (next_pkt->dts != AV_NOPTS_VALUE) {

                int wrap_bits = s->streams[next_pkt->stream_index]->pts_wrap_bits;

                

                

                int64_t last_dts = next_pkt->dts;

                while (pktl && next_pkt->pts == AV_NOPTS_VALUE) {

                    if (pktl->pkt.stream_index == next_pkt->stream_index &&

                        (av_compare_mod(next_pkt->dts, pktl->pkt.dts, 2LL << (wrap_bits - 1)) < 0)) {

                        if (av_compare_mod(pktl->pkt.pts, pktl->pkt.dts, 2LL << (wrap_bits - 1))) { 

                            next_pkt->pts = pktl->pkt.dts;

                        }

                        if (last_dts != AV_NOPTS_VALUE) {

                            

                            last_dts = pktl->pkt.dts;

                        }

                    }

                    pktl = pktl->next;

                }

                if (eof && next_pkt->pts == AV_NOPTS_VALUE && last_dts != AV_NOPTS_VALUE) {

                    

                    

                    

                    

                    

                    next_pkt->pts = last_dts + next_pkt->duration;

                }

                pktl = s->packet_buffer;

            }



            

            if (!(next_pkt->pts == AV_NOPTS_VALUE &&

                  next_pkt->dts != AV_NOPTS_VALUE && !eof)) {

                ret = read_from_packet_buffer(&s->packet_buffer,

                                               &s->packet_buffer_end, pkt);

                goto return_packet;

            }

        }



        ret = read_frame_internal(s, pkt);

        if (ret < 0) {

            if (pktl && ret != AVERROR(EAGAIN)) {

                eof = 1;

                continue;

            } else

                return ret;

        }



        if (av_dup_packet(add_to_pktbuf(&s->packet_buffer, pkt,

                          &s->packet_buffer_end)) < 0)

            return AVERROR(ENOMEM);

    }



return_packet:



    st = s->streams[pkt->stream_index];

    if (st->skip_samples) {

        uint8_t *p = av_packet_new_side_data(pkt, AV_PKT_DATA_SKIP_SAMPLES, 10);

        AV_WL32(p, st->skip_samples);

        av_log(s, AV_LOG_DEBUG, "demuxer injecting skip %d\n", st->skip_samples);

        st->skip_samples = 0;

    }



    if ((s->iformat->flags & AVFMT_GENERIC_INDEX) && pkt->flags & AV_PKT_FLAG_KEY) {

        ff_reduce_index(s, st->index);

        av_add_index_entry(st, pkt->pos, pkt->dts, 0, 0, AVINDEX_KEYFRAME);

    }



    if (is_relative(pkt->dts))

        pkt->dts -= RELATIVE_TS_BASE;

    if (is_relative(pkt->pts))

        pkt->pts -= RELATIVE_TS_BASE;



    return ret;

}