static void rv40_h_loop_filter(uint8_t *src, int stride, int dmode,

                               int lim_q1, int lim_p1,

                               int alpha, int beta, int beta2, int chroma, int edge){

    rv40_adaptive_loop_filter(src, stride, 1, dmode, lim_q1, lim_p1,

                              alpha, beta, beta2, chroma, edge);

}