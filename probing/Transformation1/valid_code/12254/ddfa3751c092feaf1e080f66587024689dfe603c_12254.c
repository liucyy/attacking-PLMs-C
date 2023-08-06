static uint8_t get_sot(J2kDecoderContext *s)

{

    if (s->buf_end - s->buf < 4)

        return AVERROR(EINVAL);



    s->curtileno = bytestream_get_be16(&s->buf); 

    if((unsigned)s->curtileno >= s->numXtiles * s->numYtiles){

        s->curtileno=0;

        return AVERROR(EINVAL);

    }



    s->buf += 4; 



    if (!bytestream_get_byte(&s->buf)){ 

        J2kTile *tile = s->tile + s->curtileno;



        

        memcpy(tile->codsty, s->codsty, s->ncomponents * sizeof(J2kCodingStyle));

        memcpy(tile->qntsty, s->qntsty, s->ncomponents * sizeof(J2kQuantStyle));

    }

    bytestream_get_byte(&s->buf); 



    return 0;

}