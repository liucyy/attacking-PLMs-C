static void compute_pkt_fields(AVFormatContext *s, AVStream *st,

                               AVCodecParserContext *pc, AVPacket *pkt)

{

    int num, den, presentation_delayed, delay, i;

    int64_t offset;



    if (s->flags & AVFMT_FLAG_NOFILLIN)

        return;



    if((s->flags & AVFMT_FLAG_IGNDTS) && pkt->pts != AV_NOPTS_VALUE)

        pkt->dts= AV_NOPTS_VALUE;



    if (st->codec->codec_id != CODEC_ID_H264 && pc && pc->pict_type == AV_PICTURE_TYPE_B)

        

        st->codec->has_b_frames = 1;



    

    delay= st->codec->has_b_frames;

    presentation_delayed = 0;



    

    

    if (delay && st->codec->active_thread_type&FF_THREAD_FRAME)

        delay -= st->codec->thread_count-1;



    

    if (delay &&

        pc && pc->pict_type != AV_PICTURE_TYPE_B)

        presentation_delayed = 1;



    if(pkt->pts != AV_NOPTS_VALUE && pkt->dts != AV_NOPTS_VALUE && pkt->dts - (1LL<<(st->pts_wrap_bits-1)) > pkt->pts && st->pts_wrap_bits<63){

        pkt->dts -= 1LL<<st->pts_wrap_bits;

    }



    

    

    

    if(delay==1 && pkt->dts == pkt->pts && pkt->dts != AV_NOPTS_VALUE && presentation_delayed){

        av_log(s, AV_LOG_DEBUG, "invalid dts/pts combination %"PRIi64"\n", pkt->dts);

        pkt->dts= pkt->pts= AV_NOPTS_VALUE;

    }



    if (pkt->duration == 0) {

        compute_frame_duration(&num, &den, st, pc, pkt);

        if (den && num) {

            pkt->duration = av_rescale_rnd(1, num * (int64_t)st->time_base.den, den * (int64_t)st->time_base.num, AV_ROUND_DOWN);



            if(pkt->duration != 0 && s->packet_buffer)

                update_initial_durations(s, st, pkt);

        }

    }



    

    if(pc && st->need_parsing == AVSTREAM_PARSE_TIMESTAMPS && pkt->size){

        

        offset = av_rescale(pc->offset, pkt->duration, pkt->size);

        if(pkt->pts != AV_NOPTS_VALUE)

            pkt->pts += offset;

        if(pkt->dts != AV_NOPTS_VALUE)

            pkt->dts += offset;

    }



    if (pc && pc->dts_sync_point >= 0) {

        

        int64_t den = st->codec->time_base.den * (int64_t) st->time_base.num;

        if (den > 0) {

            int64_t num = st->codec->time_base.num * (int64_t) st->time_base.den;

            if (pkt->dts != AV_NOPTS_VALUE) {

                

                st->reference_dts = pkt->dts - pc->dts_ref_dts_delta * num / den;

                pkt->pts = pkt->dts + pc->pts_dts_delta * num / den;

            } else if (st->reference_dts != AV_NOPTS_VALUE) {

                

                pkt->dts = st->reference_dts + pc->dts_ref_dts_delta * num / den;

                pkt->pts = pkt->dts + pc->pts_dts_delta * num / den;

            }

            if (pc->dts_sync_point > 0)

                st->reference_dts = pkt->dts; 

        }

    }



    

    if(pkt->dts != AV_NOPTS_VALUE && pkt->pts != AV_NOPTS_VALUE && pkt->pts > pkt->dts)

        presentation_delayed = 1;





    

    

    if((delay==0 || (delay==1 && pc)) && st->codec->codec_id != CODEC_ID_H264){

        if (presentation_delayed) {

            

            

            if (pkt->dts == AV_NOPTS_VALUE)

                pkt->dts = st->last_IP_pts;

            update_initial_timestamps(s, pkt->stream_index, pkt->dts, pkt->pts);

            if (pkt->dts == AV_NOPTS_VALUE)

                pkt->dts = st->cur_dts;



            

            if (st->last_IP_duration == 0)

                st->last_IP_duration = pkt->duration;

            if(pkt->dts != AV_NOPTS_VALUE)

                st->cur_dts = pkt->dts + st->last_IP_duration;

            st->last_IP_duration  = pkt->duration;

            st->last_IP_pts= pkt->pts;

            

        } else if(pkt->pts != AV_NOPTS_VALUE || pkt->dts != AV_NOPTS_VALUE || pkt->duration){

            if(pkt->pts != AV_NOPTS_VALUE && pkt->duration){

                int64_t old_diff= FFABS(st->cur_dts - pkt->duration - pkt->pts);

                int64_t new_diff= FFABS(st->cur_dts - pkt->pts);

                if(old_diff < new_diff && old_diff < (pkt->duration>>3)){

                    pkt->pts += pkt->duration;

    

                }

            }



            

            if(pkt->pts == AV_NOPTS_VALUE)

                pkt->pts = pkt->dts;

            update_initial_timestamps(s, pkt->stream_index, pkt->pts, pkt->pts);

            if(pkt->pts == AV_NOPTS_VALUE)

                pkt->pts = st->cur_dts;

            pkt->dts = pkt->pts;

            if(pkt->pts != AV_NOPTS_VALUE)

                st->cur_dts = pkt->pts + pkt->duration;

        }

    }



    if(pkt->pts != AV_NOPTS_VALUE && delay <= MAX_REORDER_DELAY){

        st->pts_buffer[0]= pkt->pts;

        for(i=0; i<delay && st->pts_buffer[i] > st->pts_buffer[i+1]; i++)

            FFSWAP(int64_t, st->pts_buffer[i], st->pts_buffer[i+1]);

        if(pkt->dts == AV_NOPTS_VALUE)

            pkt->dts= st->pts_buffer[0];

        if(st->codec->codec_id == CODEC_ID_H264){ 

            update_initial_timestamps(s, pkt->stream_index, pkt->dts, pkt->pts); 

        }

        if(pkt->dts > st->cur_dts)

            st->cur_dts = pkt->dts;

    }







    

    if(is_intra_only(st->codec))

        pkt->flags |= AV_PKT_FLAG_KEY;

    else if (pc) {

        pkt->flags = 0;

        

        if (pc->key_frame == 1)

            pkt->flags |= AV_PKT_FLAG_KEY;

        else if (pc->key_frame == -1 && pc->pict_type == AV_PICTURE_TYPE_I)

            pkt->flags |= AV_PKT_FLAG_KEY;

    }

    if (pc)

        pkt->convergence_duration = pc->convergence_duration;

}