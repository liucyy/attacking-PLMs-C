static int thp_read_packet(AVFormatContext *s,

                            AVPacket *pkt)

{

    ThpDemuxContext *thp = s->priv_data;

    AVIOContext *pb = s->pb;

    unsigned int size;

    int ret;



    if (thp->audiosize == 0) {

        

        if (thp->frame >= thp->framecnt)

            return AVERROR_EOF;



        avio_seek(pb, thp->next_frame, SEEK_SET);



        

        thp->next_frame += FFMAX(thp->next_framesz, 1);

        thp->next_framesz = avio_rb32(pb);



                        avio_rb32(pb); 

        size          = avio_rb32(pb); 



        

        if (thp->has_audio)

            thp->audiosize = avio_rb32(pb); 

        else

            thp->frame++;



        ret = av_get_packet(pb, pkt, size);



        if (ret != size) {

            av_free_packet(pkt);

            return AVERROR(EIO);

        }



        pkt->stream_index = thp->video_stream_index;

    } else {

        ret = av_get_packet(pb, pkt, thp->audiosize);



        if (ret != thp->audiosize) {

            av_free_packet(pkt);

            return AVERROR(EIO);

        }



        pkt->stream_index = thp->audio_stream_index;

        if (thp->audiosize >= 8)

            pkt->duration = AV_RB32(&pkt->data[4]);



        thp->audiosize = 0;

        thp->frame++;

    }



    return 0;

}