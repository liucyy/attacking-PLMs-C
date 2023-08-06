static int amrnb_decode_frame(AVCodecContext *avctx, void *data,

                              int *got_frame_ptr, AVPacket *avpkt)

{



    AMRContext *p = avctx->priv_data;        

    const uint8_t *buf = avpkt->data;

    int buf_size       = avpkt->size;

    float *buf_out;                          

    int i, subframe, ret;

    float fixed_gain_factor;

    AMRFixed fixed_sparse = {0};             

    float spare_vector[AMR_SUBFRAME_SIZE];   

    float synth_fixed_gain;                  

    const float *synth_fixed_vector;         



    

    p->avframe.nb_samples = AMR_BLOCK_SIZE;

    if ((ret = avctx->get_buffer(avctx, &p->avframe)) < 0) {

        av_log(avctx, AV_LOG_ERROR, "get_buffer() failed\n");

        return ret;


    buf_out = (float *)p->avframe.data[0];



    p->cur_frame_mode = unpack_bitstream(p, buf, buf_size);

    if (p->cur_frame_mode == MODE_DTX) {

        av_log_missing_feature(avctx, "dtx mode", 1);

        return -1;




    if (p->cur_frame_mode == MODE_12k2) {

        lsf2lsp_5(p);

    } else

        lsf2lsp_3(p);



    for (i = 0; i < 4; i++)

        ff_acelp_lspd2lpc(p->lsp[i], p->lpc[i], 5);



    for (subframe = 0; subframe < 4; subframe++) {

        const AMRNBSubframe *amr_subframe = &p->frame.subframe[subframe];



        decode_pitch_vector(p, amr_subframe, subframe);



        decode_fixed_sparse(&fixed_sparse, amr_subframe->pulses,

                            p->cur_frame_mode, subframe);



        

        

        

        

        decode_gains(p, amr_subframe, p->cur_frame_mode, subframe,

                     &fixed_gain_factor);



        pitch_sharpening(p, subframe, p->cur_frame_mode, &fixed_sparse);







        ff_set_fixed_vector(p->fixed_vector, &fixed_sparse, 1.0,

                            AMR_SUBFRAME_SIZE);



        p->fixed_gain[4] =

            ff_amr_set_fixed_gain(fixed_gain_factor,

                       ff_dot_productf(p->fixed_vector, p->fixed_vector,

                                       AMR_SUBFRAME_SIZE)/AMR_SUBFRAME_SIZE,

                       p->prediction_error,

                       energy_mean[p->cur_frame_mode], energy_pred_fac);



        

        

        for (i = 0; i < AMR_SUBFRAME_SIZE; i++)

            p->excitation[i] *= p->pitch_gain[4];

        ff_set_fixed_vector(p->excitation, &fixed_sparse, p->fixed_gain[4],

                            AMR_SUBFRAME_SIZE);



        

        

        

        

        

        for (i = 0; i < AMR_SUBFRAME_SIZE; i++)

            p->excitation[i] = truncf(p->excitation[i]);



        

        

        

        synth_fixed_gain = fixed_gain_smooth(p, p->lsf_q[subframe],

                                             p->lsf_avg, p->cur_frame_mode);



        synth_fixed_vector = anti_sparseness(p, &fixed_sparse, p->fixed_vector,

                                             synth_fixed_gain, spare_vector);



        if (synthesis(p, p->lpc[subframe], synth_fixed_gain,

                      synth_fixed_vector, &p->samples_in[LP_FILTER_ORDER], 0))

            

            

            

            synthesis(p, p->lpc[subframe], synth_fixed_gain,

                      synth_fixed_vector, &p->samples_in[LP_FILTER_ORDER], 1);



        postfilter(p, p->lpc[subframe], buf_out + subframe * AMR_SUBFRAME_SIZE);



        

        ff_clear_fixed_vector(p->fixed_vector, &fixed_sparse, AMR_SUBFRAME_SIZE);

        update_state(p);




    ff_acelp_apply_order_2_transfer_function(buf_out, buf_out, highpass_zeros,

                                             highpass_poles,

                                             highpass_gain * AMR_SAMPLE_SCALE,

                                             p->high_pass_mem, AMR_BLOCK_SIZE);



    

    ff_weighted_vector_sumf(p->lsf_avg, p->lsf_avg, p->lsf_q[3],

                            0.84, 0.16, LP_FILTER_ORDER);



    *got_frame_ptr   = 1;

    *(AVFrame *)data = p->avframe;



    

    return frame_sizes_nb[p->cur_frame_mode] + 1;