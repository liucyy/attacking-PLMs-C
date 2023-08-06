static int av_read_frame_internal(AVFormatContext *s, AVPacket *pkt)

{

    AVStream *st;

    int len, ret;



    for(;;) {

        

        st = s->cur_st;

        if (st) {

            if (!st->parser) {

                

                

                *pkt = s->cur_pkt;

                compute_pkt_fields(s, st, NULL, pkt);

                s->cur_st = NULL;

                return 0;

            } else if (s->cur_len > 0) {

                

                if (!st->got_frame) {

                    st->cur_frame_pts = s->cur_pkt.pts;

                    st->cur_frame_dts = s->cur_pkt.dts;

                    s->cur_pkt.pts = AV_NOPTS_VALUE;

                    s->cur_pkt.dts = AV_NOPTS_VALUE;

                    st->got_frame = 1;

                }

                len = av_parser_parse(st->parser, &st->codec, &pkt->data, &pkt->size, 

                                      s->cur_ptr, s->cur_len);

                

                s->cur_ptr += len;

                s->cur_len -= len;

                

                

                if (pkt->size) {

                    pkt->duration = 0;

                    pkt->stream_index = st->index;

                    pkt->pts = st->cur_frame_pts;

                    pkt->dts = st->cur_frame_dts;

                    pkt->destruct = av_destruct_packet_nofree;

                    compute_pkt_fields(s, st, st->parser, pkt);

                    st->got_frame = 0;

                    return 0;

                }

            } else {

                s->cur_st = NULL;

            }

        } else {

            

            if (s->cur_st && s->cur_st->parser)

                av_free_packet(&s->cur_pkt); 



            

            ret = av_read_packet(s, &s->cur_pkt);

            if (ret < 0)

                return ret;



            

            s->cur_pkt.pts = convert_timestamp_units(s, 

                                               &s->last_pkt_pts, &s->last_pkt_pts_frac,

                                               &s->last_pkt_stream_pts,

                                               s->cur_pkt.pts);

            s->cur_pkt.dts = convert_timestamp_units(s, 

                                               &s->last_pkt_dts,  &s->last_pkt_dts_frac,

                                               &s->last_pkt_stream_dts,

                                               s->cur_pkt.dts);

#if 0

            if (s->cur_pkt.stream_index == 1) {

                if (s->cur_pkt.pts != AV_NOPTS_VALUE) 

                    printf("PACKET pts=%0.3f\n", 

                           (double)s->cur_pkt.pts / AV_TIME_BASE);

                if (s->cur_pkt.dts != AV_NOPTS_VALUE) 

                    printf("PACKET dts=%0.3f\n", 

                           (double)s->cur_pkt.dts / AV_TIME_BASE);

            }

#endif

            

            

            if (s->cur_pkt.duration != 0) {

                s->cur_pkt.duration = ((int64_t)s->cur_pkt.duration * AV_TIME_BASE * s->pts_num) / 

                    s->pts_den;

            }



            st = s->streams[s->cur_pkt.stream_index];

            s->cur_st = st;

            s->cur_ptr = s->cur_pkt.data;

            s->cur_len = s->cur_pkt.size;

            if (st->need_parsing && !st->parser) {

                st->parser = av_parser_init(st->codec.codec_id);

                if (!st->parser) {

                    

                    st->need_parsing = 0;

                }

            }

        }

    }

}