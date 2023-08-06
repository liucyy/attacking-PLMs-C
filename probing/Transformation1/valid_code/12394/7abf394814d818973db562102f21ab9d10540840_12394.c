static int mov_probe(AVProbeData *p)

{

    int64_t offset;

    uint32_t tag;

    int score = 0;



    

    offset = 0;

    for (;;) {

        

        if ((offset + 8) > (unsigned int)p->buf_size)

            return score;

        tag = AV_RL32(p->buf + offset + 4);

        switch(tag) {

        

        case MKTAG('j','P',' ',' '): 

        case MKTAG('m','o','o','v'):

        case MKTAG('m','d','a','t'):

        case MKTAG('p','n','o','t'): 

        case MKTAG('u','d','t','a'): 

        case MKTAG('f','t','y','p'):

            return AVPROBE_SCORE_MAX;

        

        case MKTAG('e','d','i','w'): 

        case MKTAG('w','i','d','e'):

        case MKTAG('f','r','e','e'):

        case MKTAG('j','u','n','k'):

        case MKTAG('p','i','c','t'):

            return AVPROBE_SCORE_MAX - 5;

        case MKTAG(0x82,0x82,0x7f,0x7d):

        case MKTAG('s','k','i','p'):

        case MKTAG('u','u','i','d'):

        case MKTAG('p','r','f','l'):

            offset = AV_RB32(p->buf+offset) + offset;

            

            score = AVPROBE_SCORE_MAX - 50;

            break;

        default:

            

            return score;

        }

    }

}