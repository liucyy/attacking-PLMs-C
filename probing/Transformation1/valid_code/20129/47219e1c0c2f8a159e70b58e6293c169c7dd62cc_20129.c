static void do_video_out(AVFormatContext *s,

                         OutputStream *ost,

                         InputStream *ist,

                         AVFrame *in_picture,

                         int *frame_size, float quality)

{

    int nb_frames, i, ret, av_unused resample_changed;

    AVFrame *final_picture, *formatted_picture;

    AVCodecContext *enc, *dec;

    double sync_ipts;



    enc = ost->st->codec;

    dec = ist->st->codec;



    sync_ipts = get_sync_ipts(ost) / av_q2d(enc->time_base);



    

    nb_frames = 1;



    *frame_size = 0;



    if(video_sync_method){

        double vdelta = sync_ipts - ost->sync_opts;

        

        if (vdelta < -1.1)

            nb_frames = 0;

        else if (video_sync_method == 2 || (video_sync_method<0 && (s->oformat->flags & AVFMT_VARIABLE_FPS))){

            if(vdelta<=-0.6){

                nb_frames=0;

            }else if(vdelta>0.6)

                ost->sync_opts= lrintf(sync_ipts);

        }else if (vdelta > 1.1)

            nb_frames = lrintf(vdelta);



        if (nb_frames == 0){

            ++nb_frames_drop;

            if (verbose>2)

                fprintf(stderr, "*** drop!\n");

        }else if (nb_frames > 1) {

            nb_frames_dup += nb_frames - 1;

            if (verbose>2)

                fprintf(stderr, "*** %d dup!\n", nb_frames-1);

        }

    }else

        ost->sync_opts= lrintf(sync_ipts);



    nb_frames= FFMIN(nb_frames, max_frames[AVMEDIA_TYPE_VIDEO] - ost->frame_number);

    if (nb_frames <= 0)

        return;



    formatted_picture = in_picture;

    final_picture = formatted_picture;



#if !CONFIG_AVFILTER

    resample_changed = ost->resample_width   != dec->width  ||

                       ost->resample_height  != dec->height ||

                       ost->resample_pix_fmt != dec->pix_fmt;



    if (resample_changed) {

        av_log(NULL, AV_LOG_INFO,

               "Input stream #%d.%d frame changed from size:%dx%d fmt:%s to size:%dx%d fmt:%s\n",

               ist->file_index, ist->st->index,

               ost->resample_width, ost->resample_height, av_get_pix_fmt_name(ost->resample_pix_fmt),

               dec->width         , dec->height         , av_get_pix_fmt_name(dec->pix_fmt));

        ost->resample_width   = dec->width;

        ost->resample_height  = dec->height;

        ost->resample_pix_fmt = dec->pix_fmt;

    }



    ost->video_resample = dec->width   != enc->width  ||

                          dec->height  != enc->height ||

                          dec->pix_fmt != enc->pix_fmt;



    if (ost->video_resample) {

        final_picture = &ost->resample_frame;

        if (!ost->img_resample_ctx || resample_changed) {

            

            if (!ost->resample_frame.data[0]) {

                avcodec_get_frame_defaults(&ost->resample_frame);

                if (avpicture_alloc((AVPicture *)&ost->resample_frame, enc->pix_fmt,

                                    enc->width, enc->height)) {

                    fprintf(stderr, "Cannot allocate temp picture, check pix fmt\n");

                    exit_program(1);

                }

            }

            

            sws_freeContext(ost->img_resample_ctx);

            ost->img_resample_ctx = sws_getContext(dec->width, dec->height, dec->pix_fmt,

                                                   enc->width, enc->height, enc->pix_fmt,

                                                   ost->sws_flags, NULL, NULL, NULL);

            if (ost->img_resample_ctx == NULL) {

                fprintf(stderr, "Cannot get resampling context\n");

                exit_program(1);

            }

        }

        sws_scale(ost->img_resample_ctx, formatted_picture->data, formatted_picture->linesize,

              0, ost->resample_height, final_picture->data, final_picture->linesize);

    }

#else

    if (resample_changed) {

        avfilter_graph_free(&ost->graph);

        if (configure_video_filters(ist, ost)) {

            fprintf(stderr, "Error reinitialising filters!\n");

            exit_program(1);

        }

    }

#endif

    if (resample_changed) {

        ost->resample_width   = dec->width;

        ost->resample_height  = dec->height;

        ost->resample_pix_fmt = dec->pix_fmt;

    }



    

    for(i=0;i<nb_frames;i++) {

        AVPacket pkt;

        av_init_packet(&pkt);

        pkt.stream_index= ost->index;



        if (s->oformat->flags & AVFMT_RAWPICTURE) {

            

            AVFrame* old_frame = enc->coded_frame;

            enc->coded_frame = dec->coded_frame; 

            pkt.data= (uint8_t *)final_picture;

            pkt.size=  sizeof(AVPicture);

            pkt.pts= av_rescale_q(ost->sync_opts, enc->time_base, ost->st->time_base);

            pkt.flags |= AV_PKT_FLAG_KEY;



            write_frame(s, &pkt, ost->st->codec, ost->bitstream_filters);

            enc->coded_frame = old_frame;

        } else {

            AVFrame big_picture;



            big_picture= *final_picture;

            

            big_picture.interlaced_frame = in_picture->interlaced_frame;

            if (ost->st->codec->flags & (CODEC_FLAG_INTERLACED_DCT|CODEC_FLAG_INTERLACED_ME)) {

                if(top_field_first == -1)

                    big_picture.top_field_first = in_picture->top_field_first;

                else

                    big_picture.top_field_first = top_field_first;

            }



            

            big_picture.quality = quality;

            if(!me_threshold)

                big_picture.pict_type = 0;



            big_picture.pts= ost->sync_opts;





            if (ost->forced_kf_index < ost->forced_kf_count &&

                big_picture.pts >= ost->forced_kf_pts[ost->forced_kf_index]) {

                big_picture.pict_type = AV_PICTURE_TYPE_I;

                ost->forced_kf_index++;

            }

            ret = avcodec_encode_video(enc,

                                       bit_buffer, bit_buffer_size,

                                       &big_picture);

            if (ret < 0) {

                fprintf(stderr, "Video encoding failed\n");

                exit_program(1);

            }



            if(ret>0){

                pkt.data= bit_buffer;

                pkt.size= ret;

                if(enc->coded_frame->pts != AV_NOPTS_VALUE)

                    pkt.pts= av_rescale_q(enc->coded_frame->pts, enc->time_base, ost->st->time_base);





                if(enc->coded_frame->key_frame)

                    pkt.flags |= AV_PKT_FLAG_KEY;

                write_frame(s, &pkt, ost->st->codec, ost->bitstream_filters);

                *frame_size = ret;

                video_size += ret;

                

                

                

                if (ost->logfile && enc->stats_out) {

                    fprintf(ost->logfile, "%s", enc->stats_out);

                }

            }

        }

        ost->sync_opts++;

        ost->frame_number++;

    }

}