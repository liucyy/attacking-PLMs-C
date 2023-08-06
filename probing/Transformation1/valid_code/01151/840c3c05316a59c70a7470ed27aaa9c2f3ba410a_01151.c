static int pix_norm1_altivec(uint8_t *pix, int line_size)

{

    int i, s = 0;

    const vector unsigned int zero =

        (const vector unsigned int) vec_splat_u32(0);

    vector unsigned int sv = (vector unsigned int) vec_splat_u32(0);

    vector signed int sum;



    for (i = 0; i < 16; i++) {

        

        

        

        

        vector unsigned char pixv = vec_vsx_ld(0,  pix);



        

        sv = vec_msum(pixv, pixv, sv);



        pix += line_size;

    }

    

    sum = vec_sums((vector signed int) sv, (vector signed int) zero);

    sum = vec_splat(sum, 3);

    vec_vsx_st(sum, 0, &s);

    return s;

}