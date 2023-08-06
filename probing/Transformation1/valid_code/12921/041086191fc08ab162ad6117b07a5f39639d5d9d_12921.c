static double get_video_clock(VideoState *is)

{

    double delta;

    if (is->paused) { 

        delta = 0;

    } else {

        delta = (av_gettime() - is->video_current_pts_time) / 1000000.0;

    }

    return is->video_current_pts + delta;

}