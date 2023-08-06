static int amr_probe(AVProbeData *p)

{

    

    

    



    if (p->buf_size < 5)

        return 0;

    if(memcmp(p->buf,AMR_header,5)==0)

        return AVPROBE_SCORE_MAX;

    else

        return 0;

}