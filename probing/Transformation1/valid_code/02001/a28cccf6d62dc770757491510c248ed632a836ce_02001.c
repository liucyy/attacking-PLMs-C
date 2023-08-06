static void IMLT(float *pInput, float *pOutput, int odd_band)

{

    int     i;



    if (odd_band) {

        



        for (i=0; i<128; i++)

            FFSWAP(float, pInput[i], pInput[255-i]);

    }



    ff_imdct_calc(&mdct_ctx,pOutput,pInput);



    

    dsp.vector_fmul(pOutput,mdct_window,512);



}