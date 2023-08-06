static int mpeg_mux_end(AVFormatContext *ctx)

{

    StreamInfo *stream;

    int i;



    

    for(i=0;i<ctx->nb_streams;i++) {

        stream = ctx->streams[i]->priv_data;

        if (stream->buffer_ptr > 0) {

            if (i == (ctx->nb_streams - 1)) 

                flush_packet(ctx, i, 1);

            else

                flush_packet(ctx, i, 0);

        }

    }



    

    

    



    for(i=0;i<ctx->nb_streams;i++)

        av_freep(&ctx->streams[i]->priv_data);



    return 0;

}