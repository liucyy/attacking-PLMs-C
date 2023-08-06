static int cook_decode_init(AVCodecContext *avctx)

{

    COOKextradata *e = (COOKextradata *)avctx->extradata;

    COOKContext *q = avctx->priv_data;



    

    if (avctx->extradata_size <= 0) {

        av_log(avctx,AV_LOG_ERROR,"Necessary extradata missing!\n");

        return -1;

    } else {

        

        av_log(avctx,AV_LOG_DEBUG,"codecdata_length=%d\n",avctx->extradata_size);

        if (avctx->extradata_size >= 8){

            e->cookversion = be2me_32(e->cookversion);

            e->samples_per_frame = be2me_16(e->samples_per_frame);

            e->subbands = be2me_16(e->subbands);

        }

        if (avctx->extradata_size >= 16){

            e->js_subband_start = be2me_16(e->js_subband_start);

            e->js_vlc_bits = be2me_16(e->js_vlc_bits);

        }

    }



    

    q->sample_rate = avctx->sample_rate;

    q->nb_channels = avctx->channels;

    q->bit_rate = avctx->bit_rate;



    

    q->random_state = 1;



    

    q->samples_per_channel = e->samples_per_frame / q->nb_channels;

    q->samples_per_frame = e->samples_per_frame;

    q->subbands = e->subbands;

    q->bits_per_subpacket = avctx->block_align * 8;



    

    q->js_subband_start = 0;

    q->log2_numvector_size = 5;

    q->total_subbands = q->subbands;



    

    av_log(NULL,AV_LOG_DEBUG,"e->cookversion=%x\n",e->cookversion);

    q->joint_stereo = 0;

    switch (e->cookversion) {

        case MONO:

            if (q->nb_channels != 1) {

                av_log(avctx,AV_LOG_ERROR,"Container channels != 1, report sample!\n");

                return -1;

            }

            av_log(avctx,AV_LOG_DEBUG,"MONO\n");

            break;

        case STEREO:

            if (q->nb_channels != 1) {

                q->bits_per_subpacket = q->bits_per_subpacket/2;

            }

            av_log(avctx,AV_LOG_DEBUG,"STEREO\n");

            break;

        case JOINT_STEREO:

            if (q->nb_channels != 2) {

                av_log(avctx,AV_LOG_ERROR,"Container channels != 2, report sample!\n");

                return -1;

            }

            av_log(avctx,AV_LOG_DEBUG,"JOINT_STEREO\n");

            if (avctx->extradata_size >= 16){

                q->total_subbands = q->subbands + e->js_subband_start;

                q->js_subband_start = e->js_subband_start;

                q->joint_stereo = 1;

                q->js_vlc_bits = e->js_vlc_bits;

            }

            if (q->samples_per_channel > 256) {

                q->log2_numvector_size  = 6;

            }

            if (q->samples_per_channel > 512) {

                q->log2_numvector_size  = 7;

            }

            break;

        case MC_COOK:

            av_log(avctx,AV_LOG_ERROR,"MC_COOK not supported!\n");

            return -1;

            break;

        default:

            av_log(avctx,AV_LOG_ERROR,"Unknown Cook version, report sample!\n");

            return -1;

            break;

    }



    

    q->mlt_size = q->samples_per_channel;

    q->numvector_size = (1 << q->log2_numvector_size);



    

    init_rootpow2table(q);

    init_pow2table(q);

    init_gain_table(q);



    if (init_cook_vlc_tables(q) != 0)

        return -1;





    if(avctx->block_align >= UINT_MAX/2)

        return -1;



    

    if (q->nb_channels==2 && q->joint_stereo==0) {

        q->decoded_bytes_buffer =

          av_mallocz(avctx->block_align/2

                     + DECODE_BYTES_PAD2(avctx->block_align/2)

                     + FF_INPUT_BUFFER_PADDING_SIZE);

    } else {

        q->decoded_bytes_buffer =

          av_mallocz(avctx->block_align

                     + DECODE_BYTES_PAD1(avctx->block_align)

                     + FF_INPUT_BUFFER_PADDING_SIZE);

    }

    if (q->decoded_bytes_buffer == NULL)

        return -1;



    q->gain_ptr1[0] = &q->gain_1;

    q->gain_ptr1[1] = &q->gain_2;

    q->gain_ptr2[0] = &q->gain_3;

    q->gain_ptr2[1] = &q->gain_4;



    

    if ( init_cook_mlt(q) == 0 )

        return -1;



    

    if (q->total_subbands > 53) {

        av_log(avctx,AV_LOG_ERROR,"total_subbands > 53, report sample!\n");

        return -1;

    }

    if (q->subbands > 50) {

        av_log(avctx,AV_LOG_ERROR,"subbands > 50, report sample!\n");

        return -1;

    }

    if ((q->samples_per_channel == 256) || (q->samples_per_channel == 512) || (q->samples_per_channel == 1024)) {

    } else {

        av_log(avctx,AV_LOG_ERROR,"unknown amount of samples_per_channel = %d, report sample!\n",q->samples_per_channel);

        return -1;

    }



#ifdef COOKDEBUG

    dump_cook_context(q,e);

#endif

    return 0;

}