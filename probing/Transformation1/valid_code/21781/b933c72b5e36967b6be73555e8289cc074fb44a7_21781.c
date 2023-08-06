static void write_video_frame(AVFormatContext *oc, AVStream *st)

{

    int ret;

    static struct SwsContext *sws_ctx;

    AVCodecContext *c = st->codec;



    if (frame_count >= STREAM_NB_FRAMES) {

        

    } else {

        if (c->pix_fmt != AV_PIX_FMT_YUV420P) {

            

            if (!sws_ctx) {

                sws_ctx = sws_getContext(c->width, c->height, AV_PIX_FMT_YUV420P,

                                         c->width, c->height, c->pix_fmt,

                                         sws_flags, NULL, NULL, NULL);

                if (!sws_ctx) {

                    fprintf(stderr,

                            "Could not initialize the conversion context\n");

                    exit(1);

                }

            }

            fill_yuv_image(&src_picture, frame_count, c->width, c->height);

            sws_scale(sws_ctx,

                      (const uint8_t * const *)src_picture.data, src_picture.linesize,

                      0, c->height, dst_picture.data, dst_picture.linesize);

        } else {

            fill_yuv_image(&dst_picture, frame_count, c->width, c->height);

        }

    }



    if (oc->oformat->flags & AVFMT_RAWPICTURE) {

        

        AVPacket pkt;

        av_init_packet(&pkt);



        pkt.flags        |= AV_PKT_FLAG_KEY;

        pkt.stream_index  = st->index;

        pkt.data          = dst_picture.data[0];

        pkt.size          = sizeof(AVPicture);



        ret = av_interleaved_write_frame(oc, &pkt);

    } else {

        AVPacket pkt = { 0 };

        int got_packet;

        av_init_packet(&pkt);



        

        frame->pts = frame_count;

        ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);

        if (ret < 0) {

            fprintf(stderr, "Error encoding video frame: %s\n", av_err2str(ret));

            exit(1);

        }

        



        if (got_packet) {

            ret = write_frame(oc, &c->time_base, st, &pkt);

        } else {

            ret = 0;

        }

    }

    if (ret != 0) {

        fprintf(stderr, "Error while writing video frame: %s\n", av_err2str(ret));

        exit(1);

    }

    frame_count++;

}