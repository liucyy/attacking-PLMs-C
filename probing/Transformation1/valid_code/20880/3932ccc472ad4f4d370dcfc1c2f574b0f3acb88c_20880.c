static void get_pixels_altivec(int16_t *restrict block, const uint8_t *pixels,

                               ptrdiff_t line_size)

{

    int i;

    vec_u8 perm = vec_lvsl(0, pixels);

    const vec_u8 zero = (const vec_u8)vec_splat_u8(0);



    for (i = 0; i < 8; i++) {

        

        vec_u8 pixl = vec_ld(0, pixels);

        vec_u8 pixr = vec_ld(7, pixels);

        vec_u8 bytes = vec_perm(pixl, pixr, perm);



        

        vec_s16 shorts = (vec_s16)vec_mergeh(zero, bytes);



        

        vec_st(shorts, i * 16, (vec_s16 *)block);



        pixels += line_size;

    }

}