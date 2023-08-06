static int movie_get_frame(AVFilterLink *outlink)

{

    MovieContext *movie = outlink->src->priv;

    AVPacket pkt;

    int ret, frame_decoded;

    AVStream *st = movie->format_ctx->streams[movie->stream_index];



    if (movie->is_done == 1)

        return 0;



    while ((ret = av_read_frame(movie->format_ctx, &pkt)) >= 0) {

        

        if (pkt.stream_index == movie->stream_index) {

            avcodec_decode_video2(movie->codec_ctx, movie->frame, &frame_decoded, &pkt);



            if (frame_decoded) {

                

                movie->picref = avfilter_get_video_buffer(outlink, AV_PERM_WRITE | AV_PERM_PRESERVE |

                                                          AV_PERM_REUSE2, outlink->w, outlink->h);

                av_image_copy(movie->picref->data, movie->picref->linesize,

                              (void*)movie->frame->data,  movie->frame->linesize,

                              movie->picref->format, outlink->w, outlink->h);

                avfilter_copy_frame_props(movie->picref, movie->frame);



                

                

                movie->picref->pts = movie->frame->pkt_pts == AV_NOPTS_VALUE ?

                    movie->frame->pkt_dts : movie->frame->pkt_pts;



                if (!movie->frame->sample_aspect_ratio.num)

                    movie->picref->video->sample_aspect_ratio = st->sample_aspect_ratio;

                av_dlog(outlink->src,

                        "movie_get_frame(): file:'%s' pts:%"PRId64" time:%lf pos:%"PRId64" aspect:%d/%d\n",

                        movie->file_name, movie->picref->pts,

                        (double)movie->picref->pts * av_q2d(st->time_base),

                        movie->picref->pos,

                        movie->picref->video->sample_aspect_ratio.num,

                        movie->picref->video->sample_aspect_ratio.den);

                

                av_free_packet(&pkt);



                return 0;

            }

        }

        

        av_free_packet(&pkt);

    }



    

    

    if (ret == AVERROR_EOF)

        movie->is_done = 1;

    return ret;

}