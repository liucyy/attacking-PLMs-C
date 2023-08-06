static void throttle_fix_bucket(LeakyBucket *bkt)

{

    double min;



    

    bkt->level = bkt->burst_level = 0;



    

    min = bkt->avg / 10;

    if (bkt->avg && !bkt->max) {

        bkt->max = min;

    }

}