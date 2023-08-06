static int decode_frame(AVCodecContext *avctx, const uint8_t *databuf,

                        float **out_samples)

{

    ATRAC3Context *q = avctx->priv_data;

    int ret, i, ch;

    uint8_t *ptr1;



    if (q->coding_mode == JOINT_STEREO) {

        



        

        const uint8_t *js_databuf;

        int js_pair, js_block_align;



        js_block_align = (avctx->block_align / avctx->channels) * 2; 



        for (ch = 0; ch < avctx->channels; ch = ch + 2) {

            js_pair = ch/2;

            js_databuf = databuf + js_pair * js_block_align; 



            

            init_get_bits(&q->gb,

                          js_databuf, js_block_align * 8);



            

            ret = decode_channel_sound_unit(q, &q->gb, &q->units[ch],

                                            out_samples[ch], ch, JOINT_STEREO);

            if (ret != 0)

                return ret;



            

            if (js_databuf == q->decoded_bytes_buffer) {

                uint8_t *ptr2 = q->decoded_bytes_buffer + js_block_align - 1;

                ptr1          = q->decoded_bytes_buffer;

                for (i = 0; i < js_block_align / 2; i++, ptr1++, ptr2--)

                    FFSWAP(uint8_t, *ptr1, *ptr2);

            } else {

                const uint8_t *ptr2 = js_databuf + js_block_align - 1;

                for (i = 0; i < js_block_align; i++)

                    q->decoded_bytes_buffer[i] = *ptr2--;

            }



            

            ptr1 = q->decoded_bytes_buffer;

            for (i = 4; *ptr1 == 0xF8; i++, ptr1++) {

                if (i >= js_block_align)

                    return AVERROR_INVALIDDATA;

            }





            

            init_get_bits8(&q->gb,

                           ptr1, q->decoded_bytes_buffer + js_block_align - ptr1);



            

            memmove(q->weighting_delay[js_pair], &q->weighting_delay[js_pair][2],

                    4 * sizeof(*q->weighting_delay[js_pair]));

            q->weighting_delay[js_pair][4] = get_bits1(&q->gb);

            q->weighting_delay[js_pair][5] = get_bits(&q->gb, 3);



            for (i = 0; i < 4; i++) {

                q->matrix_coeff_index_prev[js_pair][i] = q->matrix_coeff_index_now[js_pair][i];

                q->matrix_coeff_index_now[js_pair][i]  = q->matrix_coeff_index_next[js_pair][i];

                q->matrix_coeff_index_next[js_pair][i] = get_bits(&q->gb, 2);

            }



            

            ret = decode_channel_sound_unit(q, &q->gb, &q->units[ch+1],

                                            out_samples[ch+1], ch+1, JOINT_STEREO);

            if (ret != 0)

                return ret;



            

            reverse_matrixing(out_samples[ch], out_samples[ch+1],

                              q->matrix_coeff_index_prev[js_pair],

                              q->matrix_coeff_index_now[js_pair]);



            channel_weighting(out_samples[ch], out_samples[ch+1], q->weighting_delay[js_pair]);

        }

    } else {

        

        

        for (i = 0; i < avctx->channels; i++) {

            

            init_get_bits(&q->gb,

                          databuf + i * avctx->block_align / avctx->channels,

                          avctx->block_align * 8 / avctx->channels);



            ret = decode_channel_sound_unit(q, &q->gb, &q->units[i],

                                            out_samples[i], i, q->coding_mode);

            if (ret != 0)

                return ret;

        }

    }



    

    for (i = 0; i < avctx->channels; i++) {

        float *p1 = out_samples[i];

        float *p2 = p1 + 256;

        float *p3 = p2 + 256;

        float *p4 = p3 + 256;

        ff_atrac_iqmf(p1, p2, 256, p1, q->units[i].delay_buf1, q->temp_buf);

        ff_atrac_iqmf(p4, p3, 256, p3, q->units[i].delay_buf2, q->temp_buf);

        ff_atrac_iqmf(p1, p3, 512, p1, q->units[i].delay_buf3, q->temp_buf);

    }



    return 0;

}