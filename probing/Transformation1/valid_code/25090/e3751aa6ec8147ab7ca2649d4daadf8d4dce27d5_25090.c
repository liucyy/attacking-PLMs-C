static void decode(RA288Context *ractx, float gain, int cb_coef)

{

    int i, j;

    double sumsum;

    float sum, buffer[5];



    memmove(ractx->sp_block + 5, ractx->sp_block, 36*sizeof(*ractx->sp_block));



    for (i=4; i >= 0; i--)

        ractx->sp_block[i] = -scalar_product_float(ractx->sp_block + i + 1,

                                             ractx->sp_lpc, 36);



    

    sum = 32. - scalar_product_float(ractx->gain_lpc, ractx->gain_block, 10);



    

    sum = av_clipf(sum, 0, 60);



    

    sumsum = exp(sum * 0.1151292546497) * gain; 



    for (i=0; i < 5; i++)

        buffer[i] = codetable[cb_coef][i] * sumsum;



    sum = scalar_product_float(buffer, buffer, 5) / 5;



    sum = FFMAX(sum, 1);



    

    memmove(ractx->gain_block, ractx->gain_block - 1,

            10 * sizeof(*ractx->gain_block));



    *ractx->gain_block = 10 * log10(sum) - 32;



    for (i=1; i < 5; i++)

        for (j=i-1; j >= 0; j--)

            buffer[i] -= ractx->sp_lpc[i-j-1] * buffer[j];



    

    for (i=0; i < 5; i++)

        ractx->sp_block[4-i] =

            av_clipf(ractx->sp_block[4-i] + buffer[i], -4095, 4095);

}