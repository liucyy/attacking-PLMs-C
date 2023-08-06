static int idcin_probe(AVProbeData *p)

{

    unsigned int number, sample_rate;



    



    

    if (p->buf_size < 20)

        return 0;



    

    number = AV_RL32(&p->buf[0]);

    if ((number == 0) || (number > 1024))

       return 0;



    

    number = AV_RL32(&p->buf[4]);

    if ((number == 0) || (number > 1024))

       return 0;



    

    sample_rate = AV_RL32(&p->buf[8]);

    if (sample_rate && (sample_rate < 8000 || sample_rate > 48000))

        return 0;



    

    number = AV_RL32(&p->buf[12]);

    if (number > 2 || sample_rate && !number)

        return 0;



    

    number = AV_RL32(&p->buf[16]);

    if (number > 2 || sample_rate && !number)

        return 0;



    

    return AVPROBE_SCORE_EXTENSION;

}