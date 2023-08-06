static int64_t update_scr(AVFormatContext *ctx,int stream_index,int64_t pts)

{

    MpegMuxContext *s = ctx->priv_data;

    int64_t scr;



    if (s->is_vcd)

        

        scr = 36000 + s->packet_number * 1200;

    else {

        



        

        if (stream_index == s->scr_stream_index

            && pts != AV_NOPTS_VALUE)

            scr = pts;

        else

            scr = s->last_scr;

    }



    s->last_scr=scr;



    return scr;

}