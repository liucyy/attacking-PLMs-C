static void compute_pkt_fields(AVFormatContext *s, AVStream *st, 

                               AVCodecParserContext *pc, AVPacket *pkt)

{

    int num, den, presentation_delayed;



    

    if(st->cur_dts != AV_NOPTS_VALUE){

        if(pkt->pts != AV_NOPTS_VALUE)

            pkt->pts= lsb2full(pkt->pts, st->cur_dts, st->pts_wrap_bits);

        if(pkt->dts != AV_NOPTS_VALUE)

            pkt->dts= lsb2full(pkt->dts, st->cur_dts, st->pts_wrap_bits);

    }

    

    if (pkt->duration == 0) {

        compute_frame_duration(&num, &den, s, st, pc, pkt);

        if (den && num) {

            pkt->duration = av_rescale(1, num * (int64_t)st->time_base.den, den * (int64_t)st->time_base.num);

        }

    }



    

    presentation_delayed = 0;

    if (st->codec.codec_type == CODEC_TYPE_VIDEO) {

        

        if ((st->codec.codec_id == CODEC_ID_MPEG1VIDEO ||

             st->codec.codec_id == CODEC_ID_MPEG2VIDEO ||

             st->codec.codec_id == CODEC_ID_MPEG4 ||

             st->codec.codec_id == CODEC_ID_H264) && 

            pc && pc->pict_type != FF_B_TYPE)

            presentation_delayed = 1;

        

        if(pkt->dts != AV_NOPTS_VALUE && pkt->pts != AV_NOPTS_VALUE && pkt->pts > pkt->dts)

            presentation_delayed = 1;

    }

    

    if(st->cur_dts == AV_NOPTS_VALUE){

        if(presentation_delayed) st->cur_dts = -pkt->duration;

        else                     st->cur_dts = 0;

    }





    

    if (presentation_delayed) {

        

        

        if (pkt->dts == AV_NOPTS_VALUE) {

            

            if(st->last_IP_pts != AV_NOPTS_VALUE)

                st->cur_dts = pkt->dts = st->last_IP_pts;

            else

                pkt->dts = st->cur_dts;

        } else {

            st->cur_dts = pkt->dts;

        }

        

        if (st->last_IP_duration == 0)

            st->cur_dts += pkt->duration;

        else

            st->cur_dts += st->last_IP_duration;

        st->last_IP_duration  = pkt->duration;

        st->last_IP_pts= pkt->pts;

        

    } else {

        

        if (pkt->pts == AV_NOPTS_VALUE) {

            if (pkt->dts == AV_NOPTS_VALUE) {

                pkt->pts = st->cur_dts;

                pkt->dts = st->cur_dts;

            }

            else {

                st->cur_dts = pkt->dts;

                pkt->pts = pkt->dts;

            }

        } else {

            st->cur_dts = pkt->pts;

            pkt->dts = pkt->pts;

        }

        st->cur_dts += pkt->duration;

    }



    

    

    if (pc) {

        pkt->flags = 0;

        

        switch(st->codec.codec_type) {

        case CODEC_TYPE_VIDEO:

            if (pc->pict_type == FF_I_TYPE)

                pkt->flags |= PKT_FLAG_KEY;

            break;

        case CODEC_TYPE_AUDIO:

            pkt->flags |= PKT_FLAG_KEY;

            break;

        default:

            break;

        }

    }



    

    if(pkt->pts != AV_NOPTS_VALUE)

        pkt->pts = av_rescale(pkt->pts, AV_TIME_BASE * (int64_t)st->time_base.num, st->time_base.den);

    if(pkt->dts != AV_NOPTS_VALUE)

        pkt->dts = av_rescale(pkt->dts, AV_TIME_BASE * (int64_t)st->time_base.num, st->time_base.den);



    

    pkt->duration = av_rescale(pkt->duration, AV_TIME_BASE * (int64_t)st->time_base.num, st->time_base.den);

}