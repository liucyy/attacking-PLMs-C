static int get_packet_payload_size(AVFormatContext *ctx, int stream_index,

                                   int64_t pts, int64_t dts)

{

    MpegMuxContext *s = ctx->priv_data;

    int buf_index;

    StreamInfo *stream;



    stream = ctx->streams[stream_index]->priv_data;



    buf_index = 0;

    if (((s->packet_number % s->pack_header_freq) == 0)) {

        

        if (s->is_mpeg2) 

            buf_index += 14;

        else

            buf_index += 12;

        

        if (s->is_vcd) {

            

            

            if (stream->packet_number==0)

                

                buf_index += 15;



        } else {            

            if ((s->packet_number % s->system_header_freq) == 0)

                buf_index += s->system_header_size;

        }

    }



    if (s->is_vcd && stream->packet_number==0)

        

        buf_index += s->packet_size - buf_index;

    else {

        

        buf_index += 6;

        if (s->is_mpeg2)

            buf_index += 3;

        if (pts != AV_NOPTS_VALUE) {

            if (dts != pts)

                buf_index += 5 + 5;

            else

                buf_index += 5;



        } else {

            if (!s->is_mpeg2)

                buf_index++;

        }

    

        if (stream->id < 0xc0) {

            

            buf_index += 4;

            if (stream->id >= 0xa0) {

                int n;

                buf_index += 3;

                

                n = (s->packet_size - buf_index) % stream->lpcm_align;

                if (n)

                    buf_index += (stream->lpcm_align - n);

            }

        }



        if (s->is_vcd && stream->id == AUDIO_ID)

            

            buf_index+=20;

    }

    return s->packet_size - buf_index; 

}