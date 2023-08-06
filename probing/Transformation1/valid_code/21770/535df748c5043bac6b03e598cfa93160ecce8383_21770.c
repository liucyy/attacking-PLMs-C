static void video_decode_example(const char *outfilename, const char *filename)

{

    AVCodec *codec;

    AVCodecContext *c= NULL;

    int frame, got_picture, len;

    FILE *f;

    AVFrame *picture;

    uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];

    char buf[1024];

    AVPacket avpkt;



    av_init_packet(&avpkt);



    

    memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);



    printf("Decode video file %s\n", filename);



    

    codec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);

    if (!codec) {

        fprintf(stderr, "codec not found\n");

        exit(1);

    }



    c = avcodec_alloc_context3(codec);

    picture= avcodec_alloc_frame();



    if(codec->capabilities&CODEC_CAP_TRUNCATED)

        c->flags|= CODEC_FLAG_TRUNCATED; 



    



    

    if (avcodec_open2(c, codec, NULL) < 0) {

        fprintf(stderr, "could not open codec\n");

        exit(1);

    }



    



    f = fopen(filename, "rb");

    if (!f) {

        fprintf(stderr, "could not open %s\n", filename);

        exit(1);

    }



    frame = 0;

    for(;;) {

        avpkt.size = fread(inbuf, 1, INBUF_SIZE, f);

        if (avpkt.size == 0)

            break;



        



        



        

        avpkt.data = inbuf;

        while (avpkt.size > 0) {

            len = avcodec_decode_video2(c, picture, &got_picture, &avpkt);

            if (len < 0) {

                fprintf(stderr, "Error while decoding frame %d\n", frame);

                exit(1);

            }

            if (got_picture) {

                printf("saving frame %3d\n", frame);

                fflush(stdout);



                

                snprintf(buf, sizeof(buf), outfilename, frame);

                pgm_save(picture->data[0], picture->linesize[0],

                         c->width, c->height, buf);

                frame++;

            }

            avpkt.size -= len;

            avpkt.data += len;

        }

    }



    

    avpkt.data = NULL;

    avpkt.size = 0;

    len = avcodec_decode_video2(c, picture, &got_picture, &avpkt);

    if (got_picture) {

        printf("saving last frame %3d\n", frame);

        fflush(stdout);



        

        snprintf(buf, sizeof(buf), outfilename, frame);

        pgm_save(picture->data[0], picture->linesize[0],

                 c->width, c->height, buf);

        frame++;

    }



    fclose(f);



    avcodec_close(c);

    av_free(c);

    av_free(picture);

    printf("\n");

}