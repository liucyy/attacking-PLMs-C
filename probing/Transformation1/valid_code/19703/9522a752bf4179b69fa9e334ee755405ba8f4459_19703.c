static int xsub_encode(AVCodecContext *avctx, unsigned char *buf,

                       int bufsize, void *data)

{

    AVSubtitle *h = data;

    uint64_t startTime = h->pts / 1000; 

    uint64_t endTime = startTime + h->end_display_time - h->start_display_time;

    int start_tc[4], end_tc[4];

    uint8_t *hdr = buf + 27; 

    uint8_t *rlelenptr;

    uint16_t width, height;

    int i;

    PutBitContext pb;



    if (bufsize < 27 + 7*2 + 4*3) {

        av_log(avctx, AV_LOG_ERROR, "Buffer too small for XSUB header.\n");

        return -1;

    }



    

    if (h->num_rects > 1)

        av_log(avctx, AV_LOG_WARNING, "Only single rects supported (%d in subtitle.)\n", h->num_rects);



    

    if (!h->rects[0]->pict.data[0] || !h->rects[0]->pict.data[1]) {

        av_log(avctx, AV_LOG_WARNING, "No subtitle bitmap available.\n");

        return -1;

    }



    

    if (h->rects[0]->nb_colors > 4)

        av_log(avctx, AV_LOG_WARNING, "No more than 4 subtitle colors supported (%d found.)\n", h->rects[0]->nb_colors);



    

    if (((uint32_t *)h->rects[0]->pict.data[1])[0] & 0xff)

        av_log(avctx, AV_LOG_WARNING, "Color index 0 is not transparent. Transparency will be messed up.\n");



    if (make_tc(startTime, start_tc) || make_tc(endTime, end_tc)) {

        av_log(avctx, AV_LOG_WARNING, "Time code >= 100 hours.\n");

        return -1;

    }



    snprintf(buf, 28,

        "[%02d:%02d:%02d.%03d-%02d:%02d:%02d.%03d]",

        start_tc[3], start_tc[2], start_tc[1], start_tc[0],

        end_tc[3],   end_tc[2],   end_tc[1],   end_tc[0]);



    

    

    

    

    width  = FFALIGN(h->rects[0]->w, 2) + PADDING * 2;

    height = FFALIGN(h->rects[0]->h, 2);



    bytestream_put_le16(&hdr, width);

    bytestream_put_le16(&hdr, height);

    bytestream_put_le16(&hdr, h->rects[0]->x);

    bytestream_put_le16(&hdr, h->rects[0]->y);

    bytestream_put_le16(&hdr, h->rects[0]->x + width);

    bytestream_put_le16(&hdr, h->rects[0]->y + height);



    rlelenptr = hdr; 

    hdr+=2;



    

    for (i=0; i<4; i++)

        bytestream_put_be24(&hdr, ((uint32_t *)h->rects[0]->pict.data[1])[i]);



    

    

    init_put_bits(&pb, hdr, bufsize - (hdr - buf) - 2);

    if (xsub_encode_rle(&pb, h->rects[0]->pict.data[0],

                        h->rects[0]->pict.linesize[0]*2,

                        h->rects[0]->w, (h->rects[0]->h + 1) >> 1))

        return -1;

    bytestream_put_le16(&rlelenptr, put_bits_count(&pb) >> 3); 



    if (xsub_encode_rle(&pb, h->rects[0]->pict.data[0] + h->rects[0]->pict.linesize[0],

                        h->rects[0]->pict.linesize[0]*2,

                        h->rects[0]->w, h->rects[0]->h >> 1))

        return -1;



    

    if (h->rects[0]->h & 1) {

        put_xsub_rle(&pb, h->rects[0]->w, PADDING_COLOR);

        align_put_bits(&pb);

    }



    flush_put_bits(&pb);



    return hdr - buf + put_bits_count(&pb)/8;

}