static AVStream *add_stream(AVFormatContext *oc, AVCodec **codec,

                            enum AVCodecID codec_id)

{

    AVCodecContext *c;

    AVStream *st;



    

    *codec = avcodec_find_encoder(codec_id);

    if (!(*codec)) {

        fprintf(stderr, "Could not find encoder for '%s'\n",

                avcodec_get_name(codec_id));

        exit(1);

    }



    st = avformat_new_stream(oc, *codec);

    if (!st) {

        fprintf(stderr, "Could not allocate stream\n");

        exit(1);

    }

    st->id = oc->nb_streams-1;

    c = st->codec;



    switch ((*codec)->type) {

    case AVMEDIA_TYPE_AUDIO:

        st->id = 1;

        c->sample_fmt  = AV_SAMPLE_FMT_S16;

        c->bit_rate    = 64000;

        c->sample_rate = 44100;

        c->channels    = 2;

        break;



    case AVMEDIA_TYPE_VIDEO:

        avcodec_get_context_defaults3(c, *codec);

        c->codec_id = codec_id;



        c->bit_rate = 400000;

        

        c->width    = 352;

        c->height   = 288;

        

        c->time_base.den = STREAM_FRAME_RATE;

        c->time_base.num = 1;

        c->gop_size      = 12; 

        c->pix_fmt       = STREAM_PIX_FMT;

        if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {

            

            c->max_b_frames = 2;

        }

        if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {

            

            c->mb_decision = 2;

        }

    break;



    default:

        break;

    }



    

    if (oc->oformat->flags & AVFMT_GLOBALHEADER)

        c->flags |= CODEC_FLAG_GLOBAL_HEADER;



    return st;

}