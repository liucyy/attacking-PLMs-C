void ff_rtp_send_jpeg(AVFormatContext *s1, const uint8_t *buf, int size)

{

    RTPMuxContext *s = s1->priv_data;

    const uint8_t *qtables = NULL;

    int nb_qtables = 0;

    uint8_t type = 1; 

    uint8_t w, h;

    uint8_t *p;

    int off = 0; 

    int len;

    int i;



    s->buf_ptr   = s->buf;

    s->timestamp = s->cur_timestamp;



    

    w = s1->streams[0]->codec->width  >> 3;

    h = s1->streams[0]->codec->height >> 3;



    

    if (s1->streams[0]->codec->pix_fmt == AV_PIX_FMT_YUVJ422P) {

        type = 0;

    } else if (s1->streams[0]->codec->pix_fmt == AV_PIX_FMT_YUVJ420P) {

        type = 1;

    } else {

        av_log(s1, AV_LOG_ERROR, "Unsupported pixel format\n");

        return;

    }



    

    for (i = 0; i < size; i++) {

        if (buf[i] != 0xff)

            continue;



        if (buf[i + 1] == DQT) {

            if (buf[i + 4])

                av_log(s1, AV_LOG_WARNING,

                       "Only 8-bit precision is supported.\n");



            

            nb_qtables = AV_RB16(&buf[i + 2]) / 65;

            if (i + 4 + nb_qtables * 65 > size) {

                av_log(s1, AV_LOG_ERROR, "Too short JPEG header. Aborted!\n");

                return;

            }



            qtables = &buf[i + 4];

        } else if (buf[i + 1] == SOF0) {

            if (buf[i + 14] != 17 || buf[i + 17] != 17) {

                av_log(s1, AV_LOG_ERROR,

                       "Only 1x1 chroma blocks are supported. Aborted!\n");

                return;

            }

        } else if (buf[i + 1] == SOS) {

            

            i += AV_RB16(&buf[i + 2]) + 2;

            break;

        }

    }



    

    buf  += i;

    size -= i;



    for (i = size - 2; i >= 0; i--) {

        if (buf[i] == 0xff && buf[i + 1] == EOI) {

            

            size = i;

            break;

        }

    }



    p = s->buf_ptr;

    while (size > 0) {

        int hdr_size = 8;



        if (off == 0 && nb_qtables)

            hdr_size += 4 + 64 * nb_qtables;



        

        len = FFMIN(size, s->max_payload_size - hdr_size);



        

        bytestream_put_byte(&p, 0);

        bytestream_put_be24(&p, off);

        bytestream_put_byte(&p, type);

        bytestream_put_byte(&p, 255);

        bytestream_put_byte(&p, w);

        bytestream_put_byte(&p, h);



        if (off == 0 && nb_qtables) {

            

            bytestream_put_byte(&p, 0);

            bytestream_put_byte(&p, 0);

            bytestream_put_be16(&p, 64 * nb_qtables);



            for (i = 0; i < nb_qtables; i++)

                bytestream_put_buffer(&p, &qtables[65 * i + 1], 64);

        }



        

        memcpy(p, buf, len);



        

        ff_rtp_send_data(s1, s->buf, len + hdr_size, size == len);



        buf  += len;

        size -= len;

        off  += len;

        p     = s->buf;

    }

}