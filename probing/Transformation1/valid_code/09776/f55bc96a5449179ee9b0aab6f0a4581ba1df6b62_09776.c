static int pcm_dvd_parse_header(AVCodecContext *avctx, const uint8_t *header)

{

    

    static const uint32_t frequencies[4] = { 48000, 96000, 44100, 32000 };

    PCMDVDContext *s = avctx->priv_data;

    int header_int = (header[0] & 0xe0) | (header[1] << 8) | (header[2] << 16);



    

    if (s->last_header == header_int)

        return 0;




    if (avctx->debug & FF_DEBUG_PICT_INFO)

        av_dlog(avctx, "pcm_dvd_parse_header: header = %02x%02x%02x\n",

                header[0], header[1], header[2]);

    



    

    s->extra_sample_count = 0;



    

    avctx->bits_per_coded_sample = 16 + (header[1] >> 6 & 3) * 4;

    if (avctx->bits_per_coded_sample == 28) {

        av_log(avctx, AV_LOG_ERROR,

               "PCM DVD unsupported sample depth %i\n",

               avctx->bits_per_coded_sample);

        return AVERROR_INVALIDDATA;

    }

    avctx->sample_fmt = avctx->bits_per_coded_sample == 16 ? AV_SAMPLE_FMT_S16

                                                           : AV_SAMPLE_FMT_S32;

    avctx->bits_per_raw_sample = avctx->bits_per_coded_sample;



    

    avctx->sample_rate = frequencies[header[1] >> 4 & 3];



    

    avctx->channels = 1 + (header[1] & 7);

    

    avctx->bit_rate = avctx->channels *

                      avctx->sample_rate *

                      avctx->bits_per_coded_sample;



    

    if (avctx->bits_per_coded_sample == 16) {

        s->samples_per_block = 1;

        s->block_size        = avctx->channels * 2;

    } else {

        switch (avctx->channels) {

        case 1:

        case 2:

        case 4:

            

            s->block_size        = 4 * avctx->bits_per_coded_sample / 8;

            s->samples_per_block = 4 / avctx->channels;

            s->groups_per_block  = 1;

            break;

        case 8:

            

            s->block_size        = 8 * avctx->bits_per_coded_sample / 8;

            s->samples_per_block = 1;

            s->groups_per_block  = 2;

            break;

        default:

            

            s->block_size        = 4 * avctx->channels *

                                   avctx->bits_per_coded_sample / 8;

            s->samples_per_block = 4;

            s->groups_per_block  = avctx->channels;

            break;

        }

    }



    if (avctx->debug & FF_DEBUG_PICT_INFO)

        av_dlog(avctx,

                "pcm_dvd_parse_header: %d channels, %d bits per sample, %d Hz, %d bit/s\n",

                avctx->channels, avctx->bits_per_coded_sample,

                avctx->sample_rate, avctx->bit_rate);



    s->last_header = header_int;



    return 0;

}