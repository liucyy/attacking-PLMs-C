static int mtv_probe(AVProbeData *p)

{

    if(p->buf_size < 3)

        return 0;



    



    if(*(p->buf) != 'A' || *(p->buf+1) != 'M' || *(p->buf+2) != 'V')

        return 0;



    return AVPROBE_SCORE_MAX;

}