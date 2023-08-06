static int mpeg_mux_write_packet(AVFormatContext *ctx, int stream_index,

                                 const uint8_t *buf, int size, int64_t pts)

{

    MpegMuxContext *s = ctx->priv_data;

    AVStream *st = ctx->streams[stream_index];

    StreamInfo *stream = st->priv_data;

    int64_t dts;

    int len;



    

    if (stream_index == s->scr_stream_index)

        s->last_scr = pts;

    

#if 0

    printf("%d: pts=%0.3f scr=%0.3f\n", 

           stream_index, pts / 90000.0, s->last_scr / 90000.0);

#endif

    

    

    dts = AV_NOPTS_VALUE;



    

    if (stream->start_pts == AV_NOPTS_VALUE) {

        stream->start_pts = pts;

        stream->start_dts = dts;

    }

    while (size > 0) {

        len = s->packet_data_max_size - stream->buffer_ptr;

        if (len > size)

            len = size;

        memcpy(stream->buffer + stream->buffer_ptr, buf, len);

        stream->buffer_ptr += len;

        buf += len;

        size -= len;

        while (stream->buffer_ptr >= s->packet_data_max_size) {

            

            flush_packet(ctx, stream_index,

                         stream->start_pts, stream->start_dts, s->last_scr);

            

            stream->start_pts = AV_NOPTS_VALUE;

            stream->start_dts = AV_NOPTS_VALUE;

        }

    }

    return 0;

}