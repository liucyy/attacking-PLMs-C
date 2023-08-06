static int targa_encode_frame(AVCodecContext *avctx, AVPacket *pkt,

                              const AVFrame *p, int *got_packet)

{

    int bpp, picsize, datasize = -1, ret;

    uint8_t *out;



    if(avctx->width > 0xffff || avctx->height > 0xffff) {

        av_log(avctx, AV_LOG_ERROR, "image dimensions too large\n");

        return AVERROR(EINVAL);

    }

    picsize = av_image_get_buffer_size(avctx->pix_fmt,

                                       avctx->width, avctx->height, 1);

    if ((ret = ff_alloc_packet(pkt, picsize + 45)) < 0) {

        av_log(avctx, AV_LOG_ERROR, "encoded frame too large\n");

        return ret;

    }



    

    memset(pkt->data, 0, 12);

    AV_WL16(pkt->data+12, avctx->width);

    AV_WL16(pkt->data+14, avctx->height);

    

    pkt->data[17] = 0x20 | (avctx->pix_fmt == AV_PIX_FMT_BGRA ? 8 : 0);



    switch(avctx->pix_fmt) {

    case AV_PIX_FMT_GRAY8:

        pkt->data[2]  = TGA_BW;     

        pkt->data[16] = 8;          

        break;

    case AV_PIX_FMT_RGB555LE:

        pkt->data[2]  = TGA_RGB;    

        pkt->data[16] = 16;         

        break;

    case AV_PIX_FMT_BGR24:

        pkt->data[2]  = TGA_RGB;    

        pkt->data[16] = 24;         

        break;

    case AV_PIX_FMT_BGRA:

        pkt->data[2]  = TGA_RGB;    

        pkt->data[16] = 32;         

        break;

    default:

        av_log(avctx, AV_LOG_ERROR, "Pixel format '%s' not supported.\n",

               av_get_pix_fmt_name(avctx->pix_fmt));

        return AVERROR(EINVAL);

    }

    bpp = pkt->data[16] >> 3;



    out = pkt->data + 18;  



    

    if (avctx->coder_type != FF_CODER_TYPE_RAW)

        datasize = targa_encode_rle(out, picsize, p, bpp, avctx->width, avctx->height);



    

    if(datasize >= 0)

        pkt->data[2] |= 8;



    

    else datasize = targa_encode_normal(out, p, bpp, avctx->width, avctx->height);



    out += datasize;



    

    memcpy(out, "\0\0\0\0\0\0\0\0TRUEVISION-XFILE.", 26);



    pkt->size   = out + 26 - pkt->data;

    pkt->flags |= AV_PKT_FLAG_KEY;

    *got_packet = 1;



    return 0;

}