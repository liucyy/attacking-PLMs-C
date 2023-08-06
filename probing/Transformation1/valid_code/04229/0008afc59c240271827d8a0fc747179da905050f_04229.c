DVMuxContext* dv_init_mux(AVFormatContext* s)

{

    DVMuxContext *c;

    AVStream *vst = NULL;

    int i;



    

    if (s->nb_streams > 3)

        return NULL;



    c = av_mallocz(sizeof(DVMuxContext));

    if (!c)

        return NULL;



    c->n_ast = 0;

    c->ast[0] = c->ast[1] = NULL;



    

    for (i=0; i<s->nb_streams; i++) {

         switch (s->streams[i]->codec->codec_type) {

         case CODEC_TYPE_VIDEO:

               vst = s->streams[i];

               break;

         case CODEC_TYPE_AUDIO:

             c->ast[c->n_ast++] = s->streams[i];

             break;

         default:

               goto bail_out;

         }

    }



    

    if (!vst || vst->codec->codec_id != CODEC_ID_DVVIDEO)

        goto bail_out;

    for (i=0; i<c->n_ast; i++) {

        if (c->ast[i] && (c->ast[i]->codec->codec_id != CODEC_ID_PCM_S16LE ||

                          c->ast[i]->codec->sample_rate != 48000 ||

                          c->ast[i]->codec->channels != 2))

            goto bail_out;

    }

    c->sys = dv_codec_profile(vst->codec);

    if (!c->sys)

        goto bail_out;



    if((c->n_ast > 1) && (c->sys->n_difchan < 2)) {

        

        goto bail_out;

    }



    

    c->frames = 0;

    c->has_audio = 0;

    c->has_video = 0;

    c->start_time = (time_t)s->timestamp;



    for (i=0; i<c->n_ast; i++) {

        if (c->ast[i] && av_fifo_init(&c->audio_data[i], 100*AVCODEC_MAX_AUDIO_FRAME_SIZE) < 0) {

            while (i>0) {

                i--;

                av_fifo_free(&c->audio_data[i]);

            }

            goto bail_out;

        }

    }



    return c;



bail_out:

    av_free(c);

    return NULL;

}