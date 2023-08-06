static int audio_decode_frame(VideoState *is, uint8_t *audio_buf, double *pts_ptr)

{

    AVPacket *pkt = &is->audio_pkt;

    int n, len1, data_size;

    double pts;



    for(;;) {

        

        while (is->audio_pkt_size > 0) {

            len1 = avcodec_decode_audio(&is->audio_st->codec, 

                                        (int16_t *)audio_buf, &data_size, 

                                        is->audio_pkt_data, is->audio_pkt_size);

            if (len1 < 0) {

                

                is->audio_pkt_size = 0;

                break;

            }

            

            is->audio_pkt_data += len1;

            is->audio_pkt_size -= len1;

            if (data_size <= 0)

                continue;

            

            pts = is->audio_clock;

            *pts_ptr = pts;

            n = 2 * is->audio_st->codec.channels;

printf("%f %d %d %d\n", is->audio_clock, is->audio_st->codec.channels, data_size, is->audio_st->codec.sample_rate);

            is->audio_clock += (double)data_size / 

                (double)(n * is->audio_st->codec.sample_rate);

#if defined(DEBUG_SYNC)

            {

                static double last_clock;

                printf("audio: delay=%0.3f clock=%0.3f pts=%0.3f\n",

                       is->audio_clock - last_clock,

                       is->audio_clock, pts);

                last_clock = is->audio_clock;

            }

#endif

            return data_size;

        }



        

        if (pkt->data)

            av_free_packet(pkt);

        

        if (is->paused || is->audioq.abort_request) {

            return -1;

        }

        

        

        if (packet_queue_get(&is->audioq, pkt, 1) < 0)

            return -1;

        is->audio_pkt_data = pkt->data;

        is->audio_pkt_size = pkt->size;

        

        

        if (pkt->pts != AV_NOPTS_VALUE) {

            is->audio_clock = (double)pkt->pts / AV_TIME_BASE;

        }

    }

}