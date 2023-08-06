static int mm_probe(AVProbeData *p)

{

    

    if (p->buf_size < MM_PREAMBLE_SIZE)

        return 0;

    if (AV_RL16(&p->buf[0]) != MM_TYPE_HEADER)

        return 0;

    if (AV_RL32(&p->buf[2]) != MM_HEADER_LEN_V && AV_RL32(&p->buf[2]) != MM_HEADER_LEN_AV)

        return 0;



    

    return AVPROBE_SCORE_MAX / 2;

}