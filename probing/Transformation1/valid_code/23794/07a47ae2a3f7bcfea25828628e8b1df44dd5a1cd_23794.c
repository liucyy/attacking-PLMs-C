static int flv_read_packet(AVFormatContext *s, AVPacket *pkt)

{

    int ret, i, type, size, pts, flags, is_audio, next, pos;

    AVStream *st = NULL;



 for(;;){

    pos = url_ftell(s->pb);

    url_fskip(s->pb, 4); 

    type = get_byte(s->pb);

    size = get_be24(s->pb);

    pts = get_be24(s->pb);

    pts |= get_byte(s->pb) << 24;



    if (url_feof(s->pb))

        return AVERROR(EIO);

    url_fskip(s->pb, 3); 

    flags = 0;



    if(size == 0)

        continue;



    next= size + url_ftell(s->pb);



    if (type == FLV_TAG_TYPE_AUDIO) {

        is_audio=1;

        flags = get_byte(s->pb);

    } else if (type == FLV_TAG_TYPE_VIDEO) {

        is_audio=0;

        flags = get_byte(s->pb);

    } else {

        if (type == FLV_TAG_TYPE_META && size > 13+1+4)

            flv_read_metabody(s, next);

        else 

            av_log(s, AV_LOG_ERROR, "skipping flv packet: type %d, size %d, flags %d\n", type, size, flags);

        url_fseek(s->pb, next, SEEK_SET);

        continue;

    }



    

    for(i=0;i<s->nb_streams;i++) {

        st = s->streams[i];

        if (st->id == is_audio)

            break;

    }

    if(i == s->nb_streams){

        av_log(NULL, AV_LOG_ERROR, "invalid stream\n");

        st= create_stream(s, is_audio);

        s->ctx_flags &= ~AVFMTCTX_NOHEADER;

    }



    if(  (st->discard >= AVDISCARD_NONKEY && !((flags & FLV_VIDEO_FRAMETYPE_MASK) == FLV_FRAME_KEY ||         is_audio))

       ||(st->discard >= AVDISCARD_BIDIR  &&  ((flags & FLV_VIDEO_FRAMETYPE_MASK) == FLV_FRAME_DISP_INTER && !is_audio))

       || st->discard >= AVDISCARD_ALL

       ){

        url_fseek(s->pb, next, SEEK_SET);

        continue;

    }

    if ((flags & FLV_VIDEO_FRAMETYPE_MASK) == FLV_FRAME_KEY)

        av_add_index_entry(st, pos, pts, size, 0, AVINDEX_KEYFRAME);

    break;

 }



    

    if(!url_is_streamed(s->pb) && s->duration==AV_NOPTS_VALUE){

        int size;

        const int pos= url_ftell(s->pb);

        const int fsize= url_fsize(s->pb);

        url_fseek(s->pb, fsize-4, SEEK_SET);

        size= get_be32(s->pb);

        url_fseek(s->pb, fsize-3-size, SEEK_SET);

        if(size == get_be24(s->pb) + 11){

            s->duration= get_be24(s->pb) * (int64_t)AV_TIME_BASE / 1000;

        }

        url_fseek(s->pb, pos, SEEK_SET);

    }



    if(is_audio){

        if(!st->codec->sample_rate || !st->codec->bits_per_sample || (!st->codec->codec_id && !st->codec->codec_tag)) {

            st->codec->channels = (flags & FLV_AUDIO_CHANNEL_MASK) == FLV_STEREO ? 2 : 1;

            if((flags & FLV_AUDIO_CODECID_MASK) == FLV_CODECID_NELLYMOSER_8HZ_MONO)

                st->codec->sample_rate= 8000;

            else

                st->codec->sample_rate = (44100 << ((flags & FLV_AUDIO_SAMPLERATE_MASK) >> FLV_AUDIO_SAMPLERATE_OFFSET) >> 3);

            st->codec->bits_per_sample = (flags & FLV_AUDIO_SAMPLESIZE_MASK) ? 16 : 8;

            flv_set_audio_codec(s, st, flags & FLV_AUDIO_CODECID_MASK);

        }

    }else{

        size -= flv_set_video_codec(s, st, flags & FLV_VIDEO_CODECID_MASK);

    }



    ret= av_get_packet(s->pb, pkt, size - 1);

    if (ret <= 0) {

        return AVERROR(EIO);

    }

    

    pkt->size = ret;

    pkt->pts = pts;

    pkt->stream_index = st->index;



    if (is_audio || ((flags & FLV_VIDEO_FRAMETYPE_MASK) == FLV_FRAME_KEY))

        pkt->flags |= PKT_FLAG_KEY;



    return ret;

}