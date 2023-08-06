static int mpjpeg_read_packet(AVFormatContext *s, AVPacket *pkt)

{

    int size;

    int ret;



    MPJPEGDemuxContext *mpjpeg = s->priv_data;

    if (mpjpeg->boundary == NULL) {

        mpjpeg->boundary = av_strdup("--");

        mpjpeg->searchstr = av_strdup("\r\n--");

        if (!mpjpeg->boundary || !mpjpeg->searchstr) {

            av_freep(&mpjpeg->boundary);

            av_freep(&mpjpeg->searchstr);

            return AVERROR(ENOMEM);

        }

        mpjpeg->searchstr_len = strlen(mpjpeg->searchstr);

    }



    ret = parse_multipart_header(s->pb, &size, mpjpeg->boundary, s);





    if (ret < 0)

        return ret;



    if (size > 0) {

        

        ret = av_get_packet(s->pb, pkt, size);

    } else {

        

        int remaining = 0, len;



        const int read_chunk = 2048;

        av_init_packet(pkt);

        pkt->data = NULL;

        pkt->size = 0;

        pkt->pos  = avio_tell(s->pb);



        

        ffio_ensure_seekback(s->pb, read_chunk);



        while ((ret = av_append_packet(s->pb, pkt, read_chunk - remaining)) >= 0) {

            

            len = ret + remaining;

            char *start = pkt->data + pkt->size - len;

            do {

                if (!memcmp(start, mpjpeg->searchstr, mpjpeg->searchstr_len)) {

                    

                    avio_seek(s->pb, -(len-2), SEEK_CUR);

                    pkt->size -= (len-2);

                    return pkt->size;

                }

                len--;

                start++;

            } while (len >= mpjpeg->searchstr_len);

            remaining = len;

        }



        

        if (ret == AVERROR_EOF) {

            ret = pkt->size > 0 ? pkt->size : AVERROR_EOF;

        } else {

            av_packet_unref(pkt);

        }

    }



    return ret;

}