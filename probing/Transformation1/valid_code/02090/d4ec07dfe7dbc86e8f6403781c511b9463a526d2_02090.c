int av_utf8_decode(int32_t *codep, const uint8_t **bufp, const uint8_t *buf_end,

                   unsigned int flags)

{

    const uint8_t *p = *bufp;

    uint32_t top;

    uint64_t code;

    int ret = 0;



    if (p >= buf_end)

        return 0;



    code = *p++;



    

    if ((code & 0xc0) == 0x80 || code >= 0xFE) {

        ret = AVERROR(EILSEQ);

        goto end;

    }

    top = (code & 128) >> 1;



    while (code & top) {

        int tmp;

        if (p >= buf_end) {

            (*bufp) ++;

            return AVERROR(EILSEQ); 

        }



        

        tmp = *p++ - 128;   

        if (tmp>>6) {

            (*bufp) ++;

            return AVERROR(EILSEQ);

        }

        code = (code<<6) + tmp;

        top <<= 5;

    }

    code &= (top << 1) - 1;



    if (code >= 1<<31) {

        ret = AVERROR(EILSEQ);  

        goto end;

    }



    *codep = code;



    if (code > 0x10FFFF &&

        !(flags & AV_UTF8_FLAG_ACCEPT_INVALID_BIG_CODES))

        ret = AVERROR(EILSEQ);

    if (code < 0x20 && code != 0x9 && code != 0xA && code != 0xD &&

        flags & AV_UTF8_FLAG_EXCLUDE_XML_INVALID_CONTROL_CODES)

        ret = AVERROR(EILSEQ);

    if (code >= 0xD800 && code <= 0xDFFF &&

        !(flags & AV_UTF8_FLAG_ACCEPT_SURROGATES))

        ret = AVERROR(EILSEQ);

    if ((code == 0xFFFE || code == 0xFFFF) &&

        !(flags & AV_UTF8_FLAG_ACCEPT_NON_CHARACTERS))

        ret = AVERROR(EILSEQ);



end:

    *bufp = p;

    return ret;

}