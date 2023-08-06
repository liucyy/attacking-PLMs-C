static uint8_t get_sot(Jpeg2000DecoderContext *s, int n)

{

    Jpeg2000TilePart *tp;

    uint16_t Isot;

    uint32_t Psot;

    uint8_t TPsot;



    if (s->buf_end - s->buf < 4)

        return AVERROR(EINVAL);



    Isot = bytestream_get_be16(&s->buf);        

    if (Isot) {

        av_log(s->avctx, AV_LOG_ERROR,

               "Not a DCINEMA JP2K file: more than one tile\n");

        return -1;

    }

    Psot  = bytestream_get_be32(&s->buf);       

    TPsot = bytestream_get_byte(&s->buf);       



    

    bytestream_get_byte(&s->buf);               



    tp             = s->tile[s->curtileno].tile_part + TPsot;

    tp->tile_index = Isot;

    tp->tp_len     = Psot;

    tp->tp_idx     = TPsot;



    

    if (JPEG2000_SOD == bytestream_get_be16(&s->buf))

        tp->tp_start_bstrm = s->buf;

    else {

        av_log(s->avctx, AV_LOG_ERROR, "SOD marker not found \n");

        return -1;

    }



    

    tp->tp_end_bstrm = s->buf + (tp->tp_len - n - 4);



    

    s->buf = tp->tp_end_bstrm;



    return 0;

}