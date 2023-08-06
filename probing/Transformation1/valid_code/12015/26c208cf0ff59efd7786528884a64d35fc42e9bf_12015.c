static double compute_target_delay(double delay, VideoState *is)

{

    double sync_threshold, diff;



    

    if (get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER) {

        

        diff = get_video_clock(is) - get_master_clock(is);



        

        sync_threshold = FFMAX(AV_SYNC_THRESHOLD, delay);

        if (fabs(diff) < AV_NOSYNC_THRESHOLD) {

            if (diff <= -sync_threshold)

                delay = 0;

            else if (diff >= sync_threshold)

                delay = 2 * delay;

        }

    }



    av_dlog(NULL, "video: delay=%0.3f A-V=%f\n",

            delay, -diff);



    return delay;

}