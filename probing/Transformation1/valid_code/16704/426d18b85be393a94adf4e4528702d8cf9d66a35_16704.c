static inline void idct_col (int16_t * col, int offset)

{

#define T1 13036

#define T2 27146

#define T3 43790

#define C4 23170



    static const short _T1[] ATTR_ALIGN(8) = {T1,T1,T1,T1};

    static const short _T2[] ATTR_ALIGN(8) = {T2,T2,T2,T2};

    static const short _T3[] ATTR_ALIGN(8) = {T3,T3,T3,T3};

    static const short _C4[] ATTR_ALIGN(8) = {C4,C4,C4,C4};



    

    



    movq_m2r (*_T1, mm0);               



    movq_m2r (*(col+offset+1*8), mm1);  

    movq_r2r (mm0, mm2);                



    movq_m2r (*(col+offset+7*8), mm4);  

    pmulhw_r2r (mm1, mm0);              



    movq_m2r (*_T3, mm5);               

    pmulhw_r2r (mm4, mm2);              



    movq_m2r (*(col+offset+5*8), mm6);  

    movq_r2r (mm5, mm7);                



    movq_m2r (*(col+offset+3*8), mm3);  

    psubsw_r2r (mm4, mm0);              



    movq_m2r (*_T2, mm4);               

    pmulhw_r2r (mm3, mm5);              



    paddsw_r2r (mm2, mm1);              

    pmulhw_r2r (mm6, mm7);              



    



    movq_r2r (mm4, mm2);                

    paddsw_r2r (mm3, mm5);              



    pmulhw_m2r (*(col+offset+2*8), mm4);

    paddsw_r2r (mm6, mm7);              



    psubsw_r2r (mm6, mm5);              

    paddsw_r2r (mm3, mm7);              



    movq_m2r (*(col+offset+6*8), mm3);  

    movq_r2r (mm0, mm6);                



    pmulhw_r2r (mm3, mm2);              

    psubsw_r2r (mm5, mm0);              



    psubsw_r2r (mm3, mm4);              

    paddsw_r2r (mm6, mm5);              



    movq_r2m (mm0, *(col+offset+3*8));  

    movq_r2r (mm1, mm6);                



    paddsw_m2r (*(col+offset+2*8), mm2);

    paddsw_r2r (mm7, mm6);              



    psubsw_r2r (mm7, mm1);              

    movq_r2r (mm1, mm7);                



    movq_m2r (*(col+offset+0*8), mm3);  

    paddsw_r2r (mm5, mm1);              



    movq_m2r (*_C4, mm0);               

    psubsw_r2r (mm5, mm7);              



    movq_r2m (mm6, *(col+offset+5*8));  

    pmulhw_r2r (mm0, mm1);              



    movq_r2r (mm4, mm6);                

    pmulhw_r2r (mm0, mm7);              



    movq_m2r (*(col+offset+4*8), mm5);  

    movq_r2r (mm3, mm0);                



    psubsw_r2r (mm5, mm3);              

    paddsw_r2r (mm5, mm0);              



    paddsw_r2r (mm3, mm4);              

    movq_r2r (mm0, mm5);                



    psubsw_r2r (mm6, mm3);              

    paddsw_r2r (mm2, mm5);              



    paddsw_r2r (mm1, mm1);              

    psubsw_r2r (mm2, mm0);              



    paddsw_r2r (mm7, mm7);              

    movq_r2r (mm3, mm2);                



    movq_r2r (mm4, mm6);                

    paddsw_r2r (mm7, mm3);              



    psraw_i2r (COL_SHIFT, mm3);         

    paddsw_r2r (mm1, mm4);              



    psraw_i2r (COL_SHIFT, mm4);         

    psubsw_r2r (mm1, mm6);              



    movq_m2r (*(col+offset+5*8), mm1);  

    psubsw_r2r (mm7, mm2);              



    psraw_i2r (COL_SHIFT, mm6);         

    movq_r2r (mm5, mm7);                



    movq_r2m (mm4, *(col+offset+1*8));  

    psraw_i2r (COL_SHIFT, mm2);         



    movq_r2m (mm3, *(col+offset+2*8));  

    paddsw_r2r (mm1, mm5);              



    movq_m2r (*(col+offset+3*8), mm4);  

    psubsw_r2r (mm1, mm7);              



    psraw_i2r (COL_SHIFT, mm5);         

    movq_r2r (mm0, mm3);                



    movq_r2m (mm2, *(col+offset+5*8));  

    psubsw_r2r (mm4, mm3);              



    psraw_i2r (COL_SHIFT, mm7);         

    paddsw_r2r (mm0, mm4);              



    movq_r2m (mm5, *(col+offset+0*8));  

    psraw_i2r (COL_SHIFT, mm3);         



    movq_r2m (mm6, *(col+offset+6*8));  

    psraw_i2r (COL_SHIFT, mm4);         



    movq_r2m (mm7, *(col+offset+7*8));  



    movq_r2m (mm3, *(col+offset+4*8));  



    movq_r2m (mm4, *(col+offset+3*8));  



#undef T1

#undef T2

#undef T3

#undef C4

}