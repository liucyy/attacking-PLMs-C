int avformat_seek_file(AVFormatContext *s, int stream_index, int64_t min_ts, int64_t ts, int64_t max_ts, int flags)

{

    if(min_ts > ts || max_ts < ts)

        return -1;



    ff_read_frame_flush(s);



    if (s->iformat->read_seek2)

        return s->iformat->read_seek2(s, stream_index, min_ts, ts, max_ts, flags);



    if(s->iformat->read_timestamp){

        

    }



    

    

    if(s->iformat->read_seek || 1)

        return av_seek_frame(s, stream_index, ts, flags | (ts - min_ts > (uint64_t)(max_ts - ts) ? AVSEEK_FLAG_BACKWARD : 0));



    

}