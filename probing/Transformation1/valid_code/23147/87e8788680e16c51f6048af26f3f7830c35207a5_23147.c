static int idcin_probe(AVProbeData *p)

{

    unsigned int number;



    



    

    if (p->buf_size < 20)

        return 0;



    

    number = AV_RL32(&p->buf[0]);

    if ((number == 0) || (number > 1024))

       return 0;



    

    number = AV_RL32(&p->buf[4]);

    if ((number == 0) || (number > 1024))

       return 0;



    

    number = AV_RL32(&p->buf[8]);

    if ((number != 0) && ((number < 8000) | (number > 48000)))

        return 0;



    

    number = AV_RL32(&p->buf[12]);

    if (number > 2)

        return 0;



    

    number = AV_RL32(&p->buf[16]);

    if (number > 2)

        return 0;



    

    return AVPROBE_SCORE_MAX / 2;

}