void ff_rfps_calculate(AVFormatContext *ic)

{

    int i, j;



    for (i = 0; i < ic->nb_streams; i++) {

        AVStream *st = ic->streams[i];



        if (st->codec->codec_type != AVMEDIA_TYPE_VIDEO)

            continue;

        

        

        

        if (tb_unreliable(st->codec) && st->info->duration_count > 15 && st->info->duration_gcd > FFMAX(1, st->time_base.den/(500LL*st->time_base.num)) && !st->r_frame_rate.num)

            av_reduce(&st->r_frame_rate.num, &st->r_frame_rate.den, st->time_base.den, st->time_base.num * st->info->duration_gcd, INT_MAX);

        if (st->info->duration_count>1 && !st->r_frame_rate.num

            && tb_unreliable(st->codec)) {

            int num = 0;

            double best_error= 0.01;

            AVRational ref_rate = st->r_frame_rate.num ? st->r_frame_rate : av_inv_q(st->time_base);



            for (j= 0; j<MAX_STD_TIMEBASES; j++) {

                int k;



                if (st->info->codec_info_duration && st->info->codec_info_duration*av_q2d(st->time_base) < (1001*12.0)/get_std_framerate(j))

                    continue;

                if (!st->info->codec_info_duration && 1.0 < (1001*12.0)/get_std_framerate(j))

                    continue;



                if (av_q2d(st->time_base) * st->info->rfps_duration_sum / st->info->duration_count < (1001*12.0 * 0.8)/get_std_framerate(j))

                    continue;



                for (k= 0; k<2; k++) {

                    int n = st->info->duration_count;

                    double a= st->info->duration_error[k][0][j] / n;

                    double error= st->info->duration_error[k][1][j]/n - a*a;



                    if (error < best_error && best_error> 0.000000001) {

                        best_error= error;

                        num = get_std_framerate(j);

                    }

                    if (error < 0.02)

                        av_log(ic, AV_LOG_DEBUG, "rfps: %f %f\n", get_std_framerate(j) / 12.0/1001, error);

                }

            }

            

            if (num && (!ref_rate.num || (double)num/(12*1001) < 1.01 * av_q2d(ref_rate)))

                av_reduce(&st->r_frame_rate.num, &st->r_frame_rate.den, num, 12*1001, INT_MAX);

        }

        if (   !st->avg_frame_rate.num

            && st->r_frame_rate.num && st->info->rfps_duration_sum

            && st->info->codec_info_duration <= 0

            && st->info->duration_count > 2

            && fabs(1.0 / (av_q2d(st->r_frame_rate) * av_q2d(st->time_base)) - st->info->rfps_duration_sum / (double)st->info->duration_count) <= 1.0

            ) {

            av_log(ic, AV_LOG_DEBUG, "Setting avg frame rate based on r frame rate\n");

            st->avg_frame_rate = st->r_frame_rate;

        }



        av_freep(&st->info->duration_error);

        st->info->last_dts = AV_NOPTS_VALUE;

        st->info->duration_count = 0;

        st->info->rfps_duration_sum = 0;

    }

}