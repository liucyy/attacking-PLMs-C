static int thp_read_header(AVFormatContext *s,

                           AVFormatParameters *ap)

{

    ThpDemuxContext *thp = s->priv_data;

    AVStream *st;

    AVIOContext *pb = s->pb;

    int i;



    

                           avio_rb32(pb); 

    thp->version         = avio_rb32(pb);



                           avio_rb32(pb); 

                           avio_rb32(pb); 



    thp->fps             = av_d2q(av_int2float(avio_rb32(pb)), INT_MAX);

    thp->framecnt        = avio_rb32(pb);

    thp->first_framesz   = avio_rb32(pb);

                           avio_rb32(pb); 



    thp->compoff         = avio_rb32(pb);

                           avio_rb32(pb); 

    thp->first_frame     = avio_rb32(pb);

    thp->last_frame      = avio_rb32(pb);



    thp->next_framesz    = thp->first_framesz;

    thp->next_frame      = thp->first_frame;



    

    avio_seek (pb, thp->compoff, SEEK_SET);

    thp->compcount       = avio_rb32(pb);



    

    avio_read(pb, thp->components, 16);



    for (i = 0; i < thp->compcount; i++) {

        if (thp->components[i] == 0) {

            if (thp->vst != 0)

                break;



            

            st = avformat_new_stream(s, NULL);

            if (!st)

                return AVERROR(ENOMEM);



            

            avpriv_set_pts_info(st, 64, thp->fps.den, thp->fps.num);

            st->codec->codec_type = AVMEDIA_TYPE_VIDEO;

            st->codec->codec_id = CODEC_ID_THP;

            st->codec->codec_tag = 0;  

            st->codec->width = avio_rb32(pb);

            st->codec->height = avio_rb32(pb);

            st->codec->sample_rate = av_q2d(thp->fps);

            thp->vst = st;

            thp->video_stream_index = st->index;



            if (thp->version == 0x11000)

                avio_rb32(pb); 

        } else if (thp->components[i] == 1) {

            if (thp->has_audio != 0)

                break;



            

            st = avformat_new_stream(s, NULL);

            if (!st)

                return AVERROR(ENOMEM);



            st->codec->codec_type = AVMEDIA_TYPE_AUDIO;

            st->codec->codec_id = CODEC_ID_ADPCM_THP;

            st->codec->codec_tag = 0;  

            st->codec->channels    = avio_rb32(pb); 

            st->codec->sample_rate = avio_rb32(pb); 



            avpriv_set_pts_info(st, 64, 1, st->codec->sample_rate);



            thp->audio_stream_index = st->index;

            thp->has_audio = 1;

        }

    }



    return 0;

}