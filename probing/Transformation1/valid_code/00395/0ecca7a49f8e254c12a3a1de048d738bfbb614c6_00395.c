static int fourxm_read_packet(AVFormatContext *s,

                              AVPacket *pkt)

{

    FourxmDemuxContext *fourxm = s->priv_data;

    ByteIOContext *pb = &s->pb;

    unsigned int fourcc_tag;

    unsigned int size, out_size;

    int ret = 0;

    int track_number;

    int packet_read = 0;

    unsigned char header[8];

    int64_t pts_inc;

    int audio_frame_count;



    while (!packet_read) {



        if ((ret = get_buffer(&s->pb, header, 8)) < 0)

            return ret;

        fourcc_tag = LE_32(&header[0]);

        size = LE_32(&header[4]);

        if (url_feof(pb))

            return AVERROR_IO;

        switch (fourcc_tag) {



        case LIST_TAG:

            

            fourxm->video_pts += fourxm->video_pts_inc;



            

            get_le32(pb);

            break;



        case ifrm_TAG:

        case pfrm_TAG:

        case cfrm_TAG:{



            

            if (av_new_packet(pkt, size + 8))

                return AVERROR_IO;

            pkt->stream_index = fourxm->video_stream_index;

            pkt->pts = fourxm->video_pts;

            memcpy(pkt->data, header, 8);

            ret = get_buffer(&s->pb, &pkt->data[8], size);



            if (ret < 0)

                av_free_packet(pkt);

            else

                packet_read = 1;

            break;

        }



        case snd__TAG:

            track_number = get_le32(pb);

            out_size= get_le32(pb);

            size-=8;



            if (track_number == fourxm->selected_track) {

                if (av_new_packet(pkt, size))

                    return AVERROR_IO;

                pkt->stream_index = 

                    fourxm->tracks[fourxm->selected_track].stream_index;

                pkt->pts = fourxm->audio_pts;

                ret = get_buffer(&s->pb, pkt->data, size);

                if (ret < 0)

                    av_free_packet(pkt);

                else

                    packet_read = 1;



                

                audio_frame_count = size;

                if (fourxm->tracks[fourxm->selected_track].adpcm)

                    audio_frame_count -= 

                        2 * (fourxm->tracks[fourxm->selected_track].channels);

                audio_frame_count /=

                      fourxm->tracks[fourxm->selected_track].channels;

                if (fourxm->tracks[fourxm->selected_track].adpcm)

                    audio_frame_count *= 2;

                else 

                    audio_frame_count /=

                    (fourxm->tracks[fourxm->selected_track].bits / 8);

                pts_inc = audio_frame_count;

                pts_inc *= 90000;

                pts_inc /= fourxm->tracks[fourxm->selected_track].sample_rate;

                fourxm->audio_pts += pts_inc;



            } else {

                url_fseek(pb, size, SEEK_CUR);

            }

            break;



        default:

            url_fseek(pb, size, SEEK_CUR);

            break;

        }

    }

    return ret;

}