void v_resample16_altivec(uint8_t *dst, int dst_width, const uint8_t *src,

                          int wrap, int16_t *filter)

{

    int sum, i;

    const uint8_t *s;

    vector unsigned char *tv, tmp, dstv, zero;

    vec_ss_t srchv[4], srclv[4], fv[4];

    vector signed short zeros, sumhv, sumlv;

    s = src;



    for(i=0;i<4;i++)

    {

        

        fv[i].s[0] = filter[i] << (15-FILTER_BITS);

        fv[i].v = vec_splat(fv[i].v, 0);

    }



    zero = vec_splat_u8(0);

    zeros = vec_splat_s16(0);





    

    i = (-(int)dst) & 0xf;

    while(i>0) {

        sum = s[0 * wrap] * filter[0] +

        s[1 * wrap] * filter[1] +

        s[2 * wrap] * filter[2] +

        s[3 * wrap] * filter[3];

        sum = sum >> FILTER_BITS;

        if (sum<0) sum = 0; else if (sum>255) sum=255;

        dst[0] = sum;

        dst++;

        s++;

        dst_width--;

        i--;

    }



    

    while(dst_width>=16) {

        

        tv = (vector unsigned char *) &s[0 * wrap];

        tmp = vec_perm(tv[0], tv[1], vec_lvsl(0, &s[i * wrap]));

        srchv[0].v = (vector signed short) vec_mergeh(zero, tmp);

        srclv[0].v = (vector signed short) vec_mergel(zero, tmp);

        sumhv = vec_madds(srchv[0].v, fv[0].v, zeros);

        sumlv = vec_madds(srclv[0].v, fv[0].v, zeros);



        tv = (vector unsigned char *) &s[1 * wrap];

        tmp = vec_perm(tv[0], tv[1], vec_lvsl(0, &s[1 * wrap]));

        srchv[1].v = (vector signed short) vec_mergeh(zero, tmp);

        srclv[1].v = (vector signed short) vec_mergel(zero, tmp);

        sumhv = vec_madds(srchv[1].v, fv[1].v, sumhv);

        sumlv = vec_madds(srclv[1].v, fv[1].v, sumlv);



        tv = (vector unsigned char *) &s[2 * wrap];

        tmp = vec_perm(tv[0], tv[1], vec_lvsl(0, &s[2 * wrap]));

        srchv[2].v = (vector signed short) vec_mergeh(zero, tmp);

        srclv[2].v = (vector signed short) vec_mergel(zero, tmp);

        sumhv = vec_madds(srchv[2].v, fv[2].v, sumhv);

        sumlv = vec_madds(srclv[2].v, fv[2].v, sumlv);



        tv = (vector unsigned char *) &s[3 * wrap];

        tmp = vec_perm(tv[0], tv[1], vec_lvsl(0, &s[3 * wrap]));

        srchv[3].v = (vector signed short) vec_mergeh(zero, tmp);

        srclv[3].v = (vector signed short) vec_mergel(zero, tmp);

        sumhv = vec_madds(srchv[3].v, fv[3].v, sumhv);

        sumlv = vec_madds(srclv[3].v, fv[3].v, sumlv);



        

        dstv = vec_packsu(sumhv, sumlv) ;

        vec_st(dstv, 0, (vector unsigned char *) dst);



        dst+=16;

        s+=16;

        dst_width-=16;

    }



    

    while(dst_width>0) {

        sum = s[0 * wrap] * filter[0] +

        s[1 * wrap] * filter[1] +

        s[2 * wrap] * filter[2] +

        s[3 * wrap] * filter[3];

        sum = sum >> FILTER_BITS;

        if (sum<0) sum = 0; else if (sum>255) sum=255;

        dst[0] = sum;

        dst++;

        s++;

        dst_width--;

    }

}