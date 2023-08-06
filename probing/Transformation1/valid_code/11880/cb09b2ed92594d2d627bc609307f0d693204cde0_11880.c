static void do_video_out(AVFormatContext *s, 

                         AVOutputStream *ost, 

                         AVInputStream *ist,

                         AVPicture *picture1,

                         int *frame_size)

{

    int n1, n2, nb, i, ret, frame_number, dec_frame_rate;

    AVPicture *picture, *picture2, *pict;

    AVPicture picture_tmp1, picture_tmp2;

    UINT8 *video_buffer;

    UINT8 *buf = NULL, *buf1 = NULL;

    AVCodecContext *enc, *dec;



#define VIDEO_BUFFER_SIZE (1024*1024)



    enc = &ost->st->codec;

    dec = &ist->st->codec;



    frame_number = ist->frame_number;

    dec_frame_rate = ist->st->r_frame_rate;

    

    

    n1 = ((INT64)frame_number * enc->frame_rate) / dec_frame_rate;

    n2 = (((INT64)frame_number + 1) * enc->frame_rate) / dec_frame_rate;

    nb = n2 - n1;

    if (nb <= 0) 

        return;



    video_buffer= av_malloc(VIDEO_BUFFER_SIZE);

    if(!video_buffer) return;



    

    if (do_deinterlace) {

        int size;



        

        size = avpicture_get_size(dec->pix_fmt, dec->width, dec->height);

        buf1 = av_malloc(size);

        if (!buf1)

            return;

        

        picture2 = &picture_tmp2;

        avpicture_fill(picture2, buf1, dec->pix_fmt, dec->width, dec->height);



        if (avpicture_deinterlace(picture2, picture1, 

                                  dec->pix_fmt, dec->width, dec->height) < 0) {

            

            av_free(buf1);

            buf1 = NULL;

            picture2 = picture1;

        }

    } else {

        picture2 = picture1;

    }



    

    if (enc->pix_fmt != dec->pix_fmt) {

        int size;



        

        size = avpicture_get_size(enc->pix_fmt, dec->width, dec->height);

        buf = av_malloc(size);

        if (!buf)

            return;

        pict = &picture_tmp1;

        avpicture_fill(pict, buf, enc->pix_fmt, dec->width, dec->height);

        

        if (img_convert(pict, enc->pix_fmt, 

                        picture2, dec->pix_fmt, 

                        dec->width, dec->height) < 0) {

            fprintf(stderr, "pixel format conversion not handled\n");

            goto the_end;

        }

    } else {

        pict = picture2;

    }



    

    

    if (ost->video_resample) {

        picture = &ost->pict_tmp;

        img_resample(ost->img_resample_ctx, picture, pict);

    } else {

        picture = pict;

    }

    nb=1;

    

    

    for(i=0;i<nb;i++) {

        if (enc->codec_id != CODEC_ID_RAWVIDEO) {

            

            if (same_quality) {

                enc->quality = dec->quality;

            }

            

            ret = avcodec_encode_video(enc, 

                                       video_buffer, VIDEO_BUFFER_SIZE,

                                       picture);

            

            s->oformat->write_packet(s, ost->index, video_buffer, ret, 0);

            *frame_size = ret;

            

            

            

        } else {

            write_picture(s, ost->index, picture, enc->pix_fmt, enc->width, enc->height);

        }

    }

    the_end:

    av_free(buf);

    av_free(buf1);

    av_free(video_buffer);

}