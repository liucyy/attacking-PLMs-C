static void decode_pitch_lag_high(int *lag_int, int *lag_frac, int pitch_index,

                                  uint8_t *base_lag_int, int subframe)

{

    if (subframe == 0 || subframe == 2) {

        if (pitch_index < 376) {

            *lag_int  = (pitch_index + 137) >> 2;

            *lag_frac = pitch_index - (*lag_int << 2) + 136;

        } else if (pitch_index < 440) {

            *lag_int  = (pitch_index + 257 - 376) >> 1;

            *lag_frac = (pitch_index - (*lag_int << 1) + 256 - 376) << 1;

            

        } else {

            *lag_int  = pitch_index - 280;

            *lag_frac = 0;

        }

        

        *base_lag_int = av_clip(*lag_int - 8 - (*lag_frac < 0),

                                AMRWB_P_DELAY_MIN, AMRWB_P_DELAY_MAX - 15);

        

        

        

    } else {

        *lag_int  = (pitch_index + 1) >> 2;

        *lag_frac = pitch_index - (*lag_int << 2);

        *lag_int += *base_lag_int;

    }

}