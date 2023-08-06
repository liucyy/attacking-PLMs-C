void helper_fxtract(void)

{

    CPU86_LDoubleU temp;

    unsigned int expdif;



    temp.d = ST0;

    expdif = EXPD(temp) - EXPBIAS;

    

    ST0 = expdif;

    fpush();

    BIASEXPONENT(temp);

    ST0 = temp.d;

}