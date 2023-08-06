void video_encode_example(const char *filename)

{

    AVCodec *codec;

    AVCodecContext *c= NULL;

    int i, out_size, size, x, y, outbuf_size;

    FILE *f;

    AVFrame *picture;

    uint8_t *outbuf, *picture_buf;



    printf("Video encoding\n");



    

    codec = avcodec_find_encoder(CODEC_ID_MPEG1VIDEO);

    if (!codec) {

        fprintf(stderr, "codec not found\n");

        exit(1);

    }



    c= avcodec_alloc_context();

    picture= avcodec_alloc_frame();

    

    

    c->bit_rate = 400000;

    

    c->width = 352;  

    c->height = 288;

    

    c->frame_rate = 25;  

    c->frame_rate_base= 1;

    c->gop_size = 10; 

    c->max_b_frames=1;



    

    if (avcodec_open(c, codec) < 0) {

        fprintf(stderr, "could not open codec\n");

        exit(1);

    }

    

    



    f = fopen(filename, "w");

    if (!f) {

        fprintf(stderr, "could not open %s\n", filename);

        exit(1);

    }

    

    

    outbuf_size = 100000;

    outbuf = malloc(outbuf_size);

    size = c->width * c->height;

    picture_buf = malloc((size * 3) / 2); 

    

    picture->data[0] = picture_buf;

    picture->data[1] = picture->data[0] + size;

    picture->data[2] = picture->data[1] + size / 4;

    picture->linesize[0] = c->width;

    picture->linesize[1] = c->width / 2;

    picture->linesize[2] = c->width / 2;



    

    for(i=0;i<25;i++) {

        fflush(stdout);

        

        

        for(y=0;y<c->height;y++) {

            for(x=0;x<c->width;x++) {

                picture->data[0][y * picture->linesize[0] + x] = x + y + i * 3;

            }

        }



        

        for(y=0;y<c->height/2;y++) {

            for(x=0;x<c->width/2;x++) {

                picture->data[1][y * picture->linesize[1] + x] = 128 + y + i * 2;

                picture->data[2][y * picture->linesize[2] + x] = 64 + x + i * 5;

            }

        }



        

        out_size = avcodec_encode_video(c, outbuf, outbuf_size, picture);

        printf("encoding frame %3d (size=%5d)\n", i, out_size);

        fwrite(outbuf, 1, out_size, f);

    }



    

    for(; out_size; i++) {

        fflush(stdout);

        

        out_size = avcodec_encode_video(c, outbuf, outbuf_size, NULL);

        printf("write frame %3d (size=%5d)\n", i, out_size);

        fwrite(outbuf, 1, out_size, f);

    }



    

    outbuf[0] = 0x00;

    outbuf[1] = 0x00;

    outbuf[2] = 0x01;

    outbuf[3] = 0xb7;

    fwrite(outbuf, 1, 4, f);

    fclose(f);

    free(picture_buf);

    free(outbuf);



    avcodec_close(c);

    free(c);

    free(picture);

    printf("\n");

}