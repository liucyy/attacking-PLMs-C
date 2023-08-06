int av_find_stream_info(AVFormatContext *ic)

{

    int i, count, ret, read_size, j;

    AVStream *st;

    AVPacket pkt1, *pkt;

    AVPacketList *pktl=NULL, **ppktl;

    int64_t last_dts[MAX_STREAMS];

    int duration_count[MAX_STREAMS]={0};

    double duration_error[MAX_STREAMS][MAX_STD_TIMEBASES]={{0}}; 

    offset_t old_offset = url_ftell(&ic->pb);



    for(i=0;i<ic->nb_streams;i++) {

        st = ic->streams[i];

        if(st->codec->codec_type == CODEC_TYPE_VIDEO){



            if(!st->codec->time_base.num)

                st->codec->time_base= st->time_base;

        }

        

        if (!st->parser) {

            st->parser = av_parser_init(st->codec->codec_id);

            if(st->need_parsing == 2 && st->parser){

                st->parser->flags |= PARSER_FLAG_COMPLETE_FRAMES;

            }

        }

    }



    for(i=0;i<MAX_STREAMS;i++){

        last_dts[i]= AV_NOPTS_VALUE;

    }



    count = 0;

    read_size = 0;

    ppktl = &ic->packet_buffer;

    for(;;) {

        

        for(i=0;i<ic->nb_streams;i++) {

            st = ic->streams[i];

            if (!has_codec_parameters(st->codec))

                break;

            

            if(   st->codec->time_base.den >= 101LL*st->codec->time_base.num

               && duration_count[i]<20 && st->codec->codec_type == CODEC_TYPE_VIDEO)

                break;

            if(st->parser && st->parser->parser->split && !st->codec->extradata)

                break;

        }

        if (i == ic->nb_streams) {

            

            if (!(ic->ctx_flags & AVFMTCTX_NOHEADER)) {

                

                ret = count;

                break;

            }

        }

        

        if (read_size >= MAX_READ_SIZE) {

            ret = count;

            break;

        }



        

        ret = av_read_frame_internal(ic, &pkt1);

        if (ret < 0) {

            

            ret = -1; 

            for(i=0;i<ic->nb_streams;i++) {

                st = ic->streams[i];

                if (!has_codec_parameters(st->codec)){

                    char buf[256];

                    avcodec_string(buf, sizeof(buf), st->codec, 0);

                    av_log(ic, AV_LOG_INFO, "Could not find codec parameters (%s)\n", buf);

                } else {

                    ret = 0;

                }

            }

            break;

        }



        pktl = av_mallocz(sizeof(AVPacketList));

        if (!pktl) {

            ret = AVERROR_NOMEM;

            break;

        }



        

        *ppktl = pktl;

        ppktl = &pktl->next;



        pkt = &pktl->pkt;

        *pkt = pkt1;



        

        if (av_dup_packet(pkt) < 0) {

            ret = AVERROR_NOMEM;

            break;

        }



        read_size += pkt->size;



        st = ic->streams[pkt->stream_index];

        if(st->codec_info_nb_frames>1) 

            st->codec_info_duration += pkt->duration;

        if (pkt->duration != 0)

            st->codec_info_nb_frames++;



        {

            int index= pkt->stream_index;

            int64_t last= last_dts[index];

            int64_t duration= pkt->dts - last;



            if(pkt->dts != AV_NOPTS_VALUE && last != AV_NOPTS_VALUE && duration>0){

                double dur= duration * av_q2d(st->time_base);







                if(duration_count[index] < 2)

                    memset(duration_error, 0, sizeof(duration_error));

                for(i=1; i<MAX_STD_TIMEBASES; i++){

                    int framerate= get_std_framerate(i);

                    int ticks= lrintf(dur*framerate/(1001*12));

                    double error= dur - ticks*1001*12/(double)framerate;

                    duration_error[index][i] += error*error;

                }

                duration_count[index]++;



                if(st->codec_info_nb_frames == 0 && 0)

                    st->codec_info_duration += duration;

            }

            if(last == AV_NOPTS_VALUE || duration_count[index]<=1)

                last_dts[pkt->stream_index]= pkt->dts;

        }

        if(st->parser && st->parser->parser->split && !st->codec->extradata){

            int i= st->parser->parser->split(st->codec, pkt->data, pkt->size);

            if(i){

                st->codec->extradata_size= i;

                st->codec->extradata= av_malloc(st->codec->extradata_size + FF_INPUT_BUFFER_PADDING_SIZE);

                memcpy(st->codec->extradata, pkt->data, st->codec->extradata_size);

                memset(st->codec->extradata + i, 0, FF_INPUT_BUFFER_PADDING_SIZE);

            }

        }



        

        if (!has_codec_parameters(st->codec) )

            try_decode_frame(st, pkt->data, pkt->size);



        if (av_rescale_q(st->codec_info_duration, st->time_base, AV_TIME_BASE_Q) >= ic->max_analyze_duration) {

            break;

        }

        count++;

    }



    

    for(i=0;i<ic->nb_streams;i++) {

        st = ic->streams[i];

        if(st->codec->codec)

            avcodec_close(st->codec);

    }

    for(i=0;i<ic->nb_streams;i++) {

        st = ic->streams[i];

        if (st->codec->codec_type == CODEC_TYPE_VIDEO) {

            if(st->codec->codec_id == CODEC_ID_RAWVIDEO && !st->codec->codec_tag && !st->codec->bits_per_sample)

                st->codec->codec_tag= avcodec_pix_fmt_to_codec_tag(st->codec->pix_fmt);



            if(duration_count[i]

               && (st->codec->time_base.num*101LL <= st->codec->time_base.den || st->codec->codec_id == CODEC_ID_MPEG2VIDEO) ){

                double best_error= 2*av_q2d(st->time_base);

                best_error= best_error*best_error*duration_count[i]*1000*12*30;



                for(j=1; j<MAX_STD_TIMEBASES; j++){

                    double error= duration_error[i][j] * get_std_framerate(j);





                    if(error < best_error){

                        best_error= error;

                        av_reduce(&st->r_frame_rate.num, &st->r_frame_rate.den, get_std_framerate(j), 12*1001, INT_MAX);

                    }

                }

            }



            if (!st->r_frame_rate.num){

                if(    st->codec->time_base.den * (int64_t)st->time_base.num

                    <= st->codec->time_base.num * (int64_t)st->time_base.den){

                    st->r_frame_rate.num = st->codec->time_base.den;

                    st->r_frame_rate.den = st->codec->time_base.num;

                }else{

                    st->r_frame_rate.num = st->time_base.den;

                    st->r_frame_rate.den = st->time_base.num;

                }

            }

        }else if(st->codec->codec_type == CODEC_TYPE_AUDIO) {

            if(!st->codec->bits_per_sample)

                st->codec->bits_per_sample= av_get_bits_per_sample(st->codec->codec_id);

        }

    }



    av_estimate_timings(ic, old_offset);

#if 0

    

    for(i=0;i<ic->nb_streams;i++) {

        st = ic->streams[i];

        if (st->codec->codec_type == CODEC_TYPE_VIDEO) {

            if(b-frames){

                ppktl = &ic->packet_buffer;

                while(ppkt1){

                    if(ppkt1->stream_index != i)

                        continue;

                    if(ppkt1->pkt->dts < 0)

                        break;

                    if(ppkt1->pkt->pts != AV_NOPTS_VALUE)

                        break;

                    ppkt1->pkt->dts -= delta;

                    ppkt1= ppkt1->next;

                }

                if(ppkt1)

                    continue;

                st->cur_dts -= delta;

            }

        }

    }

#endif

    return ret;

}