static int sub2video_prepare(InputStream *ist)

{

    AVFormatContext *avf = input_files[ist->file_index]->ctx;

    int i, w, h;



    

    w = ist->dec_ctx->width;

    h = ist->dec_ctx->height;

    if (!(w && h)) {

        for (i = 0; i < avf->nb_streams; i++) {

            if (avf->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {

                w = FFMAX(w, avf->streams[i]->codec->width);

                h = FFMAX(h, avf->streams[i]->codec->height);

            }

        }

        if (!(w && h)) {

            w = FFMAX(w, 720);

            h = FFMAX(h, 576);

        }

        av_log(avf, AV_LOG_INFO, "sub2video: using %dx%d canvas\n", w, h);

    }

    ist->sub2video.w = ist->dec_ctx->width  = ist->resample_width  = w;

    ist->sub2video.h = ist->dec_ctx->height = ist->resample_height = h;



    

    ist->resample_pix_fmt = ist->dec_ctx->pix_fmt = AV_PIX_FMT_RGB32;



    ist->sub2video.frame = av_frame_alloc();

    if (!ist->sub2video.frame)

        return AVERROR(ENOMEM);

    ist->sub2video.last_pts = INT64_MIN;

    return 0;

}