void video_decode_example(const char *outfilename, const char *filename)

{

    AVCodec *codec;

    AVCodecContext *c= NULL;

    int frame, size, got_picture, len;

    FILE *f;

    AVFrame *picture;

    uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE], *inbuf_ptr;

    char buf[1024];



    

    memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);



    printf("Video decoding\n");



    

    codec = avcodec_find_decoder(CODEC_ID_MPEG1VIDEO);

    if (!codec) {

        fprintf(stderr, "codec not found\n");

        exit(1);

    }



    c= avcodec_alloc_context();

    picture= avcodec_alloc_frame();



    if(codec->capabilities&CODEC_CAP_TRUNCATED)

        c->flags|= CODEC_FLAG_TRUNCATED; 



    



    

    if (avcodec_open(c, codec) < 0) {

        fprintf(stderr, "could not open codec\n");

        exit(1);

    }

    

    



    f = fopen(filename, "r");

    if (!f) {

        fprintf(stderr, "could not open %s\n", filename);

        exit(1);

    }

    

    frame = 0;

    for(;;) {

        size = fread(inbuf, 1, INBUF_SIZE, f);

        if (size == 0)

            break;



        



        



        

        inbuf_ptr = inbuf;

        while (size > 0) {

            len = avcodec_decode_video(c, picture, &got_picture, 

                                       inbuf_ptr, size);

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

            size -= len;

            inbuf_ptr += len;

        }

    }



    

    len = avcodec_decode_video(c, picture, &got_picture, 

                               NULL, 0);

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

    free(c);

    free(picture);

    printf("\n");

}