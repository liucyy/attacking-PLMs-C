static int put_cod(Jpeg2000EncoderContext *s)

{

    Jpeg2000CodingStyle *codsty = &s->codsty;



    if (s->buf_end - s->buf < 14)

        return -1;



    bytestream_put_be16(&s->buf, JPEG2000_COD);

    bytestream_put_be16(&s->buf, 12); 

    bytestream_put_byte(&s->buf, 0);  

    

    bytestream_put_byte(&s->buf, 0); 

    bytestream_put_be16(&s->buf, 1); 

    if(s->avctx->pix_fmt == AV_PIX_FMT_YUV444P){

        bytestream_put_byte(&s->buf, 2); 

    }else{

        bytestream_put_byte(&s->buf, 0); 

    }

    

    bytestream_put_byte(&s->buf, codsty->nreslevels - 1); 

    bytestream_put_byte(&s->buf, codsty->log2_cblk_width-2); 

    bytestream_put_byte(&s->buf, codsty->log2_cblk_height-2); 

    bytestream_put_byte(&s->buf, 0); 

    bytestream_put_byte(&s->buf, codsty->transform); 

    return 0;

}