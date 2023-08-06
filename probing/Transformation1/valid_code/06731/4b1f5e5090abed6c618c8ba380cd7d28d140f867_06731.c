static int qdm2_decode(QDM2Context *q, const uint8_t *in, int16_t *out)

{

    int ch, i;

    const int frame_size = (q->frame_size * q->channels);



    

    q->compressed_data = in;

    q->compressed_size = q->checksum_size;



    

    memmove(q->output_buffer, &q->output_buffer[frame_size], frame_size * sizeof(float));

    memset(&q->output_buffer[frame_size], 0, frame_size * sizeof(float));



    

    if (q->sub_packet == 0) {

        q->has_errors = 0; 

        av_log(NULL,AV_LOG_DEBUG,"Superblock follows\n");

        qdm2_decode_super_block(q);

    }



    

    if (!q->has_errors) {

        if (q->sub_packet == 2)

            qdm2_decode_fft_packets(q);



        qdm2_fft_tone_synthesizer(q, q->sub_packet);

    }



    

    for (ch = 0; ch < q->channels; ch++) {

        qdm2_calculate_fft(q, ch, q->sub_packet);



        if (!q->has_errors && q->sub_packet_list_C[0].packet != NULL) {

            SAMPLES_NEEDED_2("has errors, and C list is not empty")

            return -1;

        }

    }



    

    if (!q->has_errors && q->do_synth_filter)

        qdm2_synthesis_filter(q, q->sub_packet);



    q->sub_packet = (q->sub_packet + 1) % 16;



    

    for (i = 0; i < frame_size; i++) {

        int value = (int)q->output_buffer[i];



        if (value > SOFTCLIP_THRESHOLD)

            value = (value >  HARDCLIP_THRESHOLD) ?  32767 :  softclip_table[ value - SOFTCLIP_THRESHOLD];

        else if (value < -SOFTCLIP_THRESHOLD)

            value = (value < -HARDCLIP_THRESHOLD) ? -32767 : -softclip_table[-value - SOFTCLIP_THRESHOLD];



        out[i] = value;

    }



    return 0;

}