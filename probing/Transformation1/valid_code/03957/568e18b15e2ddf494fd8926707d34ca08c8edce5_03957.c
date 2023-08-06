static int mov_read_stts(MOVContext *c, ByteIOContext *pb, MOV_atom_t atom)

{

    AVStream *st = c->fc->streams[c->fc->nb_streams-1];

    

    int entries, i;

    int64_t duration=0;

    int64_t total_sample_count=0;



    print_atom("stts", atom);



    get_byte(pb); 

    get_byte(pb); get_byte(pb); get_byte(pb); 

    entries = get_be32(pb);



    c->streams[c->fc->nb_streams-1]->stts_count = entries;

    c->streams[c->fc->nb_streams-1]->stts_data = (uint64_t*) av_malloc(entries * sizeof(uint64_t));



#ifdef DEBUG

av_log(NULL, AV_LOG_DEBUG, "track[%i].stts.entries = %i\n", c->fc->nb_streams-1, entries);

#endif

    for(i=0; i<entries; i++) {

        int32_t sample_duration;

        int32_t sample_count;



        sample_count=get_be32(pb);

        sample_duration = get_be32(pb);

        c->streams[c->fc->nb_streams - 1]->stts_data[i] = (uint64_t)sample_count<<32 | (uint64_t)sample_duration;

#ifdef DEBUG

        av_log(NULL, AV_LOG_DEBUG, "sample_count=%d, sample_duration=%d\n",sample_count,sample_duration);

#endif

        duration+=sample_duration*sample_count;

        total_sample_count+=sample_count;



#if 0 



        if (!i && st->codec.codec_type==CODEC_TYPE_VIDEO) {

            st->codec.frame_rate_base = sample_duration ? sample_duration : 1;

            st->codec.frame_rate = c->streams[c->fc->nb_streams-1]->time_scale;

#ifdef DEBUG

            av_log(NULL, AV_LOG_DEBUG, "VIDEO FRAME RATE= %i (sd= %i)\n", st->codec.frame_rate, sample_duration);

#endif

        }

#endif

    }



    

    if(duration>0)

    {

        av_reduce(

            &st->codec.frame_rate, 

            &st->codec.frame_rate_base, 

            c->streams[c->fc->nb_streams-1]->time_scale * total_sample_count,

            duration,

            INT_MAX

        );



#ifdef DEBUG

        av_log(NULL, AV_LOG_DEBUG, "FRAME RATE average (video or audio)= %f (tot sample count= %i ,tot dur= %i timescale=%d)\n", (float)st->codec.frame_rate/st->codec.frame_rate_base,total_sample_count,duration,c->streams[c->fc->nb_streams-1]->time_scale);

#endif

    }

    else

    {

        st->codec.frame_rate_base = 1;

        st->codec.frame_rate = c->streams[c->fc->nb_streams-1]->time_scale;

    }

    return 0;

}