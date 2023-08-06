static void av_always_inline filter_mb_edgecv( uint8_t *pix, int stride, const int16_t bS[4], unsigned int qp, H264Context *h ) {

    const int qp_bd_offset = 6 * (h->sps.bit_depth_luma - 8);

    const unsigned int index_a = qp - qp_bd_offset + h->slice_alpha_c0_offset;

    const int alpha = alpha_table[index_a];

    const int beta  = beta_table[qp - qp_bd_offset + h->slice_beta_offset];

    if (alpha ==0 || beta == 0) return;



    if( bS[0] < 4 ) {

        int8_t tc[4];

        tc[0] = tc0_table[index_a][bS[0]]+1;

        tc[1] = tc0_table[index_a][bS[1]]+1;

        tc[2] = tc0_table[index_a][bS[2]]+1;

        tc[3] = tc0_table[index_a][bS[3]]+1;

        h->h264dsp.h264_h_loop_filter_chroma(pix, stride, alpha, beta, tc);

    } else {

        h->h264dsp.h264_h_loop_filter_chroma_intra(pix, stride, alpha, beta);

    }

}