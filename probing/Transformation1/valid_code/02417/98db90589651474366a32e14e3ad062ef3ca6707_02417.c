int avformat_seek_file(AVFormatContext *s, int stream_index, int64_t min_ts, int64_t ts, int64_t max_ts, int flags)

{

    if(min_ts > ts || max_ts < ts)

        return -1;



    if (s->iformat->read_seek2) {

        int ret;

        ff_read_frame_flush(s);



        if (stream_index == -1 && s->nb_streams == 1) {

            AVRational time_base = s->streams[0]->time_base;

            ts = av_rescale_q(ts, AV_TIME_BASE_Q, time_base);

            min_ts = av_rescale_rnd(min_ts, time_base.den,

                                    time_base.num * (int64_t)AV_TIME_BASE,

                                    AV_ROUND_UP);

            max_ts = av_rescale_rnd(max_ts, time_base.den,

                                    time_base.num * (int64_t)AV_TIME_BASE,

                                    AV_ROUND_DOWN);

        }



        ret = s->iformat->read_seek2(s, stream_index, min_ts, ts, max_ts, flags);



        if (ret >= 0)

            avformat_queue_attached_pictures(s);

        return ret;

    }



    if(s->iformat->read_timestamp){

        

    }



    

    

    if (s->iformat->read_seek || 1) {

        int dir = (ts - (uint64_t)min_ts > (uint64_t)max_ts - ts ? AVSEEK_FLAG_BACKWARD : 0);

        int ret = av_seek_frame(s, stream_index, ts, flags | dir);

        if (ret<0 && ts != min_ts && max_ts != ts) {

            ret = av_seek_frame(s, stream_index, dir ? max_ts : min_ts, flags | dir);

            if (ret >= 0)

                ret = av_seek_frame(s, stream_index, ts, flags | (dir^AVSEEK_FLAG_BACKWARD));

        }

        return ret;

    }



    

}