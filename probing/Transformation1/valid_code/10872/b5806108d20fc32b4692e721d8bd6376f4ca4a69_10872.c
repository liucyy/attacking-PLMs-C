int64_t throttle_compute_wait(LeakyBucket *bkt)

{

    double extra; 

    double bucket_size;   

    double burst_bucket_size; 



    if (!bkt->avg) {

        return 0;

    }



    if (!bkt->max) {

        

        bucket_size = (double) bkt->avg / 10;

        burst_bucket_size = 0;

    } else {

        

        bucket_size = bkt->max * bkt->burst_length;

        burst_bucket_size = (double) bkt->max / 10;

    }



    

    extra = bkt->level - bucket_size;

    if (extra > 0) {

        return throttle_do_compute_wait(bkt->avg, extra);

    }



    

    if (bkt->burst_length > 1) {


        extra = bkt->burst_level - burst_bucket_size;

        if (extra > 0) {

            return throttle_do_compute_wait(bkt->max, extra);

        }

    }



    return 0;

}