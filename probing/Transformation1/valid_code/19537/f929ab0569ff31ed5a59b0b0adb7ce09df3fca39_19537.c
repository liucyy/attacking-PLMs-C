static int encode_dvd_subtitles(uint8_t *outbuf, int outbuf_size,

                                const AVSubtitle *h)

{

    uint8_t *q, *qq;

    int object_id;

    int offset1[20], offset2[20];

    int i, imax, color, alpha, rects = h->num_rects;

    unsigned long hmax;

    unsigned long hist[256];

    int           cmap[256];



    if (rects == 0 || h->rects == NULL)

        return -1;

    if (rects > 20)

        rects = 20;



    

    for (i=0; i<256; ++i) {

        hist[i] = 0;

        cmap[i] = 0;

    }

    for (object_id = 0; object_id < rects; object_id++)

        for (i=0; i<h->rects[object_id]->w*h->rects[object_id]->h; ++i) {

            color = h->rects[object_id]->pict.data[0][i];

            

            alpha = ((uint32_t*)h->rects[object_id]->pict.data[1])[color] >> 24;

            hist[color] += alpha;

        }

    for (color=3;; --color) {

        hmax = 0;

        imax = 0;

        for (i=0; i<256; ++i)

            if (hist[i] > hmax) {

                imax = i;

                hmax = hist[i];

            }

        if (hmax == 0)

            break;

        if (color == 0)

            color = 3;

        av_log(NULL, AV_LOG_DEBUG, "dvd_subtitle hist[%d]=%ld -> col %d\n",

               imax, hist[imax], color);

        cmap[imax] = color;

        hist[imax] = 0;

    }





    

    q = outbuf + 4;

    for (object_id = 0; object_id < rects; object_id++) {

        offset1[object_id] = q - outbuf;

        

        if ((q - outbuf) + h->rects[object_id]->w*h->rects[object_id]->h/2

            + 17*rects + 21 > outbuf_size) {

            av_log(NULL, AV_LOG_ERROR, "dvd_subtitle too big\n");

            return -1;

        }

        dvd_encode_rle(&q, h->rects[object_id]->pict.data[0],

                       h->rects[object_id]->w*2,

                       h->rects[object_id]->w, h->rects[object_id]->h >> 1,

                       cmap);

        offset2[object_id] = q - outbuf;

        dvd_encode_rle(&q, h->rects[object_id]->pict.data[0] + h->rects[object_id]->w,

                       h->rects[object_id]->w*2,

                       h->rects[object_id]->w, h->rects[object_id]->h >> 1,

                       cmap);

    }



    

    qq = outbuf + 2;

    bytestream_put_be16(&qq, q - outbuf);



    

    bytestream_put_be16(&q, (h->start_display_time*90) >> 10);

    bytestream_put_be16(&q, (q - outbuf)  + 8 + 12*rects + 2);

    *q++ = 0x03; 

    *q++ = 0x03; *q++ = 0x7f;

    *q++ = 0x04; 

    *q++ = 0xf0; *q++ = 0x00;

    /);

    *q++ = 0x02; 

    *q++ = 0xff; 



    qq = outbuf;

    bytestream_put_be16(&qq, q - outbuf);



    av_log(NULL, AV_LOG_DEBUG, "subtitle_packet size=%td\n", q - outbuf);

    return q - outbuf;

}