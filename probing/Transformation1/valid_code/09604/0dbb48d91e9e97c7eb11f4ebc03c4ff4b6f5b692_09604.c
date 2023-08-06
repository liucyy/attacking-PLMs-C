static int mpeg_mux_end(AVFormatContext *ctx)

{

    MpegMuxContext *s = ctx->priv_data;

    StreamInfo *stream;

    int i;



    

    for(i=0;i<ctx->nb_streams;i++) {

        stream = ctx->streams[i]->priv_data;

        while (stream->buffer_ptr > 0) {

            flush_packet(ctx, i, AV_NOPTS_VALUE, AV_NOPTS_VALUE, s->last_scr);

        }

    }



    

    

    



    for(i=0;i<ctx->nb_streams;i++)

        av_freep(&ctx->streams[i]->priv_data);



    return 0;

}