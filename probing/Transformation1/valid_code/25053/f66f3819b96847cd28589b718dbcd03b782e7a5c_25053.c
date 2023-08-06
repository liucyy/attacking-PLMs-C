static int vid_probe(AVProbeData *p)
{
    
    if (AV_RL32(p->buf) != MKTAG('V', 'I', 'D', 0))
        return 0;
    return AVPROBE_SCORE_MAX;
}