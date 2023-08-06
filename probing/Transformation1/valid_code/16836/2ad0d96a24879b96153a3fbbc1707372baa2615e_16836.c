void ff_acelp_interpolate(

        int16_t* out,

        const int16_t* in,

        const int16_t* filter_coeffs,

        int precision,

        int frac_pos,

        int filter_length,

        int length)

{

    int n, i;



    assert(pitch_delay_frac >= 0 && pitch_delay_frac < precision);



    for(n=0; n<length; n++)

    {

        int idx = 0;

        int v = 0x4000;



        for(i=0; i<filter_length;)

        {



            



            



            v += in[n + i] * filter_coeffs[idx + frac_pos];

            idx += precision;

            i++;

            v += in[n - i] * filter_coeffs[idx - frac_pos];

        }

        out[n] = av_clip_int16(v >> 15);

    }

}