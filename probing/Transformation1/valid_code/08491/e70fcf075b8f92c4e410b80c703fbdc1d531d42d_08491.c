AVStream *add_video_stream(AVFormatContext *oc, int codec_id)

{

    AVCodec *codec;

    AVCodecContext *c;

    AVStream *st;

    uint8_t *picture_buf;

    int size;



    st = av_new_stream(oc, 0);

    if (!st) {

        fprintf(stderr, "Could not alloc stream\n");

        exit(1);

    }

    

    

    codec = avcodec_find_encoder(codec_id);

    if (!codec) {

        fprintf(stderr, "codec not found\n");

        exit(1);

    }



    c = &st->codec;

    c->codec_type = CODEC_TYPE_VIDEO;



    

    c->bit_rate = 400000;

    

    c->width = 352;  

    c->height = 288;

    

    c->frame_rate = 25;  

    c->frame_rate_base= 1;

    c->gop_size = 12; 



    

    if (avcodec_open(c, codec) < 0) {

        fprintf(stderr, "could not open codec\n");

        exit(1);

    }



    

    picture= avcodec_alloc_frame();

    video_outbuf_size = 100000;

    video_outbuf = malloc(video_outbuf_size);



    size = c->width * c->height;

    picture_buf = malloc((size * 3) / 2); 

    

    picture->data[0] = picture_buf;

    picture->data[1] = picture->data[0] + size;

    picture->data[2] = picture->data[1] + size / 4;

    picture->linesize[0] = c->width;

    picture->linesize[1] = c->width / 2;

    picture->linesize[2] = c->width / 2;



    return st;

}