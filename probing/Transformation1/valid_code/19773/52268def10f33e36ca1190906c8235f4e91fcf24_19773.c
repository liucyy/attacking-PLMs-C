static int thp_probe(AVProbeData *p)

{

    

    if (AV_RL32(p->buf) == MKTAG('T', 'H', 'P', '\0'))

        return AVPROBE_SCORE_MAX;

    else

        return 0;

}