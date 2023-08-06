static void get_pixels_altivec(int16_t *restrict block, const uint8_t *pixels, int line_size)

{

    int i;

    vector unsigned char perm = vec_lvsl(0, pixels);

    vector unsigned char bytes;

    const vector unsigned char zero = (const vector unsigned char)vec_splat_u8(0);

    vector signed short shorts;



    for (i = 0; i < 8; i++) {

        

        

        

        vector unsigned char pixl = vec_ld( 0, pixels);

        vector unsigned char pixr = vec_ld(15, pixels);

        bytes = vec_perm(pixl, pixr, perm);



        

        shorts = (vector signed short)vec_mergeh(zero, bytes);



        

        vec_st(shorts, i*16, (vector signed short*)block);



        pixels += line_size;

    }

}