void swr_compensate(struct SwrContext *s, int sample_delta, int compensation_distance){

    ResampleContext *c= s->resample;



    c->compensation_distance= compensation_distance;

    c->dst_incr = c->ideal_dst_incr - c->ideal_dst_incr * (int64_t)sample_delta / compensation_distance;

}