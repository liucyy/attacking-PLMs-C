int avpriv_mpegaudio_decode_header(MPADecodeHeader *s, uint32_t header)

{

    int sample_rate, frame_size, mpeg25, padding;

    int sample_rate_index, bitrate_index;

    if (header & (1<<20)) {

        s->lsf = (header & (1<<19)) ? 0 : 1;

        mpeg25 = 0;

    } else {

        s->lsf = 1;

        mpeg25 = 1;

    }



    s->layer = 4 - ((header >> 17) & 3);

    

    sample_rate_index = (header >> 10) & 3;



    sample_rate = avpriv_mpa_freq_tab[sample_rate_index] >> (s->lsf + mpeg25);

    sample_rate_index += 3 * (s->lsf + mpeg25);

    s->sample_rate_index = sample_rate_index;

    s->error_protection = ((header >> 16) & 1) ^ 1;

    s->sample_rate = sample_rate;



    bitrate_index = (header >> 12) & 0xf;

    padding = (header >> 9) & 1;

    

    s->mode = (header >> 6) & 3;

    s->mode_ext = (header >> 4) & 3;

    

    

    



    if (s->mode == MPA_MONO)

        s->nb_channels = 1;

    else

        s->nb_channels = 2;



    if (bitrate_index != 0) {

        frame_size = avpriv_mpa_bitrate_tab[s->lsf][s->layer - 1][bitrate_index];

        s->bit_rate = frame_size * 1000;

        switch(s->layer) {

        case 1:

            frame_size = (frame_size * 12000) / sample_rate;

            frame_size = (frame_size + padding) * 4;

            break;

        case 2:

            frame_size = (frame_size * 144000) / sample_rate;

            frame_size += padding;

            break;

        default:

        case 3:

            frame_size = (frame_size * 144000) / (sample_rate << s->lsf);

            frame_size += padding;

            break;

        }

        s->frame_size = frame_size;

    } else {

        

        return 1;

    }



#if defined(DEBUG)

    av_dlog(NULL, "layer%d, %d Hz, %d kbits/s, ",

           s->layer, s->sample_rate, s->bit_rate);

    if (s->nb_channels == 2) {

        if (s->layer == 3) {

            if (s->mode_ext & MODE_EXT_MS_STEREO)

                av_dlog(NULL, "ms-");

            if (s->mode_ext & MODE_EXT_I_STEREO)

                av_dlog(NULL, "i-");

        }

        av_dlog(NULL, "stereo");

    } else {

        av_dlog(NULL, "mono");

    }

    av_dlog(NULL, "\n");

#endif

    return 0;

}