static void lowpass_line_complex_c(uint8_t *dstp, ptrdiff_t width, const uint8_t *srcp,

                                   ptrdiff_t mref, ptrdiff_t pref)

{

    const uint8_t *srcp_above = srcp + mref;

    const uint8_t *srcp_below = srcp + pref;

    const uint8_t *srcp_above2 = srcp + mref * 2;

    const uint8_t *srcp_below2 = srcp + pref * 2;

    int i;

    for (i = 0; i < width; i++) {

        

        

        

        dstp[i] = av_clip_uint8((4 + (srcp[i] << 2)

                  + ((srcp[i] + srcp_above[i] + srcp_below[i]) << 1)

                  - srcp_above2[i] - srcp_below2[i]) >> 3);

    }

}