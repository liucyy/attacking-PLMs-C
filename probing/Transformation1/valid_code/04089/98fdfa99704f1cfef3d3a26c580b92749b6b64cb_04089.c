static int sad8_altivec(void *v, uint8_t *pix1, uint8_t *pix2, int line_size, int h)

{

    int i;

    int s;

    const vector unsigned int zero = (const vector unsigned int)vec_splat_u32(0);

    const vector unsigned char permclear = (vector unsigned char){255,255,255,255,255,255,255,255,0,0,0,0,0,0,0,0};

    vector unsigned char perm1 = vec_lvsl(0, pix1);

    vector unsigned char perm2 = vec_lvsl(0, pix2);

    vector unsigned char t1, t2, t3,t4, t5;

    vector unsigned int sad;

    vector signed int sumdiffs;



    sad = (vector unsigned int)vec_splat_u32(0);



    for (i = 0; i < h; i++) {

        

        vector unsigned char pix1l = vec_ld( 0, pix1);

        vector unsigned char pix1r = vec_ld(15, pix1);

        vector unsigned char pix2l = vec_ld( 0, pix2);

        vector unsigned char pix2r = vec_ld(15, pix2);

        t1 = vec_and(vec_perm(pix1l, pix1r, perm1), permclear);

        t2 = vec_and(vec_perm(pix2l, pix2r, perm2), permclear);



        

        t3 = vec_max(t1, t2);

        t4 = vec_min(t1, t2);

        t5 = vec_sub(t3, t4);



        

        sad = vec_sum4s(t5, sad);



        pix1 += line_size;

        pix2 += line_size;

    }



    

    sumdiffs = vec_sums((vector signed int) sad, (vector signed int) zero);

    sumdiffs = vec_splat(sumdiffs, 3);

    vec_ste(sumdiffs, 0, &s);



    return s;

}