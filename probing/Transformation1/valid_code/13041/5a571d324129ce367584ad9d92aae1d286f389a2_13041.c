static int parse_h264_sdp_line(AVFormatContext *s, int st_index,

                               PayloadContext *h264_data, const char *line)

{

    AVStream *stream;

    AVCodecContext *codec;

    const char *p = line;



    if (st_index < 0)

        return 0;



    stream = s->streams[st_index];

    codec = stream->codec;

    assert(h264_data->cookie == MAGIC_COOKIE);



    if (av_strstart(p, "framesize:", &p)) {

        char buf1[50];

        char *dst = buf1;



        

        while (*p && *p == ' ') p++; 

        while (*p && *p != ' ') p++; 

        while (*p && *p == ' ') p++; 

        while (*p && *p != '-' && (dst - buf1) < sizeof(buf1) - 1) {

            *dst++ = *p++;

        }

        *dst = '\0';



        

        

        codec->width = atoi(buf1);

        codec->height = atoi(p + 1); 

        codec->pix_fmt = PIX_FMT_YUV420P;

    } else if (av_strstart(p, "fmtp:", &p)) {

        return ff_parse_fmtp(stream, h264_data, p, sdp_parse_fmtp_config_h264);

    } else if (av_strstart(p, "cliprect:", &p)) {

        

    }



    return 0;                   

}