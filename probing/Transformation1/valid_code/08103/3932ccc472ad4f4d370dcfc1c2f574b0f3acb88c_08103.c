static void diff_pixels_altivec(int16_t *restrict block, const uint8_t *s1,

                                const uint8_t *s2, int stride)

{

    int i;

    vec_u8 perm1 = vec_lvsl(0, s1);

    vec_u8 perm2 = vec_lvsl(0, s2);

    const vec_u8 zero = (const vec_u8)vec_splat_u8(0);

    vec_s16 shorts1, shorts2;



    for (i = 0; i < 4; i++) {

        

        vec_u8 pixl  = vec_ld(0,  s1);

        vec_u8 pixr  = vec_ld(15, s1);

        vec_u8 bytes = vec_perm(pixl, pixr, perm1);



        

        shorts1 = (vec_s16)vec_mergeh(zero, bytes);



        

        pixl  = vec_ld(0,  s2);

        pixr  = vec_ld(15, s2);

        bytes = vec_perm(pixl, pixr, perm2);



        

        shorts2 = (vec_s16)vec_mergeh(zero, bytes);



        

        shorts1 = vec_sub(shorts1, shorts2);



        

        vec_st(shorts1, 0, (vec_s16 *)block);



        s1    += stride;

        s2    += stride;

        block += 8;



        



        

        pixl  = vec_ld(0,  s1);

        pixr  = vec_ld(15, s1);

        bytes = vec_perm(pixl, pixr, perm1);



        

        shorts1 = (vec_s16)vec_mergeh(zero, bytes);



        

        pixl  = vec_ld(0,  s2);

        pixr  = vec_ld(15, s2);

        bytes = vec_perm(pixl, pixr, perm2);



        

        shorts2 = (vec_s16)vec_mergeh(zero, bytes);



        

        shorts1 = vec_sub(shorts1, shorts2);



        

        vec_st(shorts1, 0, (vec_s16 *)block);



        s1    += stride;

        s2    += stride;

        block += 8;

    }

}