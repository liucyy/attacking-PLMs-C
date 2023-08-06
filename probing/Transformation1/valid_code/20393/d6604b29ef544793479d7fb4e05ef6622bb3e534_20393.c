static int libschroedinger_encode_close(AVCodecContext *avctx)

{

    SchroEncoderParams *p_schro_params = avctx->priv_data;



    

    schro_encoder_free(p_schro_params->encoder);



    

    ff_schro_queue_free(&p_schro_params->enc_frame_queue,

                        libschroedinger_free_frame);





    

    if (p_schro_params->enc_buf_size)

        av_freep(&p_schro_params->enc_buf);



    

    av_freep(&p_schro_params->format);



    av_frame_free(&avctx->coded_frame);



    return 0;

}