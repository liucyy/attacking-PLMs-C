static int fourxm_read_header(AVFormatContext *s,

                              AVFormatParameters *ap)

{

    ByteIOContext *pb = s->pb;

    unsigned int fourcc_tag;

    unsigned int size;

    int header_size;

    FourxmDemuxContext *fourxm = s->priv_data;

    unsigned char *header;

    int i;

    int current_track = -1;

    AVStream *st;



    fourxm->track_count = 0;

    fourxm->tracks = NULL;

    fourxm->selected_track = 0;

    fourxm->fps = 1.0;



    

    url_fseek(pb, 12, SEEK_CUR);



    

    GET_LIST_HEADER();

    header_size = size - 4;

    if (fourcc_tag != HEAD_TAG || size < 4)

        return AVERROR_INVALIDDATA;



    

    header = av_malloc(header_size);

    if (!header)

        return AVERROR(ENOMEM);

    if (get_buffer(pb, header, header_size) != header_size)

        return AVERROR(EIO);



    

    for (i = 0; i < header_size - 8; i++) {

        fourcc_tag = AV_RL32(&header[i]);

        size = AV_RL32(&header[i + 4]);



        if (fourcc_tag == std__TAG) {

            fourxm->fps = av_int2flt(AV_RL32(&header[i + 12]));

        } else if (fourcc_tag == vtrk_TAG) {

            

            if (size != vtrk_SIZE) {

                av_free(header);

                return AVERROR_INVALIDDATA;

            }

            fourxm->width = AV_RL32(&header[i + 36]);

            fourxm->height = AV_RL32(&header[i + 40]);



            

            st = av_new_stream(s, 0);

            if (!st)

                return AVERROR(ENOMEM);

            av_set_pts_info(st, 60, 1, fourxm->fps);



            fourxm->video_stream_index = st->index;



            st->codec->codec_type = CODEC_TYPE_VIDEO;

            st->codec->codec_id = CODEC_ID_4XM;

            st->codec->extradata_size = 4;

            st->codec->extradata = av_malloc(4);

            AV_WL32(st->codec->extradata, AV_RL32(&header[i + 16]));

            st->codec->width = fourxm->width;

            st->codec->height = fourxm->height;



            i += 8 + size;

        } else if (fourcc_tag == strk_TAG) {

            

            if (size != strk_SIZE) {

                av_free(header);

                return AVERROR_INVALIDDATA;

            }

            current_track = AV_RL32(&header[i + 8]);

            if (current_track + 1 > fourxm->track_count) {

                fourxm->track_count = current_track + 1;

                if((unsigned)fourxm->track_count >= UINT_MAX / sizeof(AudioTrack))

                    return -1;

                fourxm->tracks = av_realloc(fourxm->tracks,

                    fourxm->track_count * sizeof(AudioTrack));

                if (!fourxm->tracks) {

                    av_free(header);

                    return AVERROR(ENOMEM);

                }

            }

            fourxm->tracks[current_track].adpcm = AV_RL32(&header[i + 12]);

            fourxm->tracks[current_track].channels = AV_RL32(&header[i + 36]);

            fourxm->tracks[current_track].sample_rate = AV_RL32(&header[i + 40]);

            fourxm->tracks[current_track].bits = AV_RL32(&header[i + 44]);

            i += 8 + size;



            

            st = av_new_stream(s, current_track);

            if (!st)

                return AVERROR(ENOMEM);



            av_set_pts_info(st, 60, 1, fourxm->tracks[current_track].sample_rate);



            fourxm->tracks[current_track].stream_index = st->index;



            st->codec->codec_type = CODEC_TYPE_AUDIO;

            st->codec->codec_tag = 0;

            st->codec->channels = fourxm->tracks[current_track].channels;

            st->codec->sample_rate = fourxm->tracks[current_track].sample_rate;

            st->codec->bits_per_coded_sample = fourxm->tracks[current_track].bits;

            st->codec->bit_rate = st->codec->channels * st->codec->sample_rate *

                st->codec->bits_per_coded_sample;

            st->codec->block_align = st->codec->channels * st->codec->bits_per_coded_sample;

            if (fourxm->tracks[current_track].adpcm)

                st->codec->codec_id = CODEC_ID_ADPCM_4XM;

            else if (st->codec->bits_per_coded_sample == 8)

                st->codec->codec_id = CODEC_ID_PCM_U8;

            else

                st->codec->codec_id = CODEC_ID_PCM_S16LE;

        }

    }



    av_free(header);



    

    GET_LIST_HEADER();

    if (fourcc_tag != MOVI_TAG)

        return AVERROR_INVALIDDATA;



    

    fourxm->video_pts = -1;  

    fourxm->audio_pts = 0;



    return 0;

}