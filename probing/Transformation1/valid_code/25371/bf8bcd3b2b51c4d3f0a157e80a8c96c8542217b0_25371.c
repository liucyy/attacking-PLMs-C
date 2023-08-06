static int read_packet(AVFormatContext* ctx, AVPacket *pkt)

{

    al_data *ad = ctx->priv_data;

    int error=0;

    const char *error_msg;

    ALCint nb_samples;



    

    alcGetIntegerv(ad->device, ALC_CAPTURE_SAMPLES, (ALCsizei) sizeof(ALCint), &nb_samples);

    if (error = al_get_error(ad->device, &error_msg)) goto fail;



    

    av_new_packet(pkt, nb_samples*ad->sample_step);

    pkt->pts = av_gettime();



    

    alcCaptureSamples(ad->device, pkt->data, nb_samples);

    if (error = al_get_error(ad->device, &error_msg)) goto fail;



    return pkt->size;

fail:

    

    if (pkt->data)

        av_destruct_packet(pkt);

    if (error_msg)

        av_log(ctx, AV_LOG_ERROR, "Error: %s\n", error_msg);

    return error;

}