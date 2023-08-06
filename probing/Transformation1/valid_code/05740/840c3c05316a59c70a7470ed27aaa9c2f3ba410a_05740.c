static int pix_sum_altivec(uint8_t *pix, int line_size)

{

    int i, s;

    const vector unsigned int zero =

        (const vector unsigned int) vec_splat_u32(0);

    vector unsigned int sad = (vector unsigned int) vec_splat_u32(0);

    vector signed int sumdiffs;



    for (i = 0; i < 16; i++) {

        

        

        

        

        vector unsigned char t1   = vec_vsx_ld(0,  pix);



        

        sad = vec_sum4s(t1, sad);



        pix += line_size;

    }



    

    sumdiffs = vec_sums((vector signed int) sad, (vector signed int) zero);

    sumdiffs = vec_splat(sumdiffs, 3);

    vec_vsx_st(sumdiffs, 0, &s);

    return s;

}