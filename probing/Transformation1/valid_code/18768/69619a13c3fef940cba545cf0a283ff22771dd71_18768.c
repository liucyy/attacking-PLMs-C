static int matroska_probe(AVProbeData *p)

{

    uint64_t total = 0;

    int len_mask = 0x80, size = 1, n = 1, i;



    

    if (AV_RB32(p->buf) != EBML_ID_HEADER)

        return 0;



    

    total = p->buf[4];

    while (size <= 8 && !(total & len_mask)) {

        size++;

        len_mask >>= 1;

    }

    if (size > 8)

      return 0;

    total &= (len_mask - 1);

    while (n < size)

        total = (total << 8) | p->buf[4 + n++];



    

    if (p->buf_size < 4 + size + total)

      return 0;



    

    for (i = 0; i < FF_ARRAY_ELEMS(matroska_doctypes); i++) {

        int probelen = strlen(matroska_doctypes[i]);



        for (n = 4+size; n <= 4+size+total-probelen; n++)

            if (!memcmp(p->buf+n, matroska_doctypes[i], probelen))

                return AVPROBE_SCORE_MAX;

    }



    

    return AVPROBE_SCORE_MAX/2;

}