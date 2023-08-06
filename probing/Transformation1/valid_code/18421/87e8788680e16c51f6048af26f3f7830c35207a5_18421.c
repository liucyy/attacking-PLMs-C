static int vmd_probe(AVProbeData *p)

{

    if (p->buf_size < 2)

        return 0;



    

    if (AV_RL16(&p->buf[0]) != VMD_HEADER_SIZE - 2)

        return 0;



    

    return AVPROBE_SCORE_MAX / 2;

}