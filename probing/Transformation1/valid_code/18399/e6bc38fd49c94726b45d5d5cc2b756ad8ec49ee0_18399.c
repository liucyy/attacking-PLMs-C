static void wmv2_idct_col(short * b)

{

    int s1,s2;

    int a0,a1,a2,a3,a4,a5,a6,a7;

    

    a1 = (W1*b[8*1]+W7*b[8*7] + 4)>>3;

    a7 = (W7*b[8*1]-W1*b[8*7] + 4)>>3;

    a5 = (W5*b[8*5]+W3*b[8*3] + 4)>>3;

    a3 = (W3*b[8*5]-W5*b[8*3] + 4)>>3;

    a2 = (W2*b[8*2]+W6*b[8*6] + 4)>>3;

    a6 = (W6*b[8*2]-W2*b[8*6] + 4)>>3;

    a0 = (W0*b[8*0]+W0*b[8*4]    )>>3;

    a4 = (W0*b[8*0]-W0*b[8*4]    )>>3;

    

    s1 = (181*(a1-a5+a7-a3)+128)>>8;

    s2 = (181*(a1-a5-a7+a3)+128)>>8;

    

    b[8*0] = (a0+a2+a1+a5 + (1<<13))>>14;

    b[8*1] = (a4+a6 +s1   + (1<<13))>>14;

    b[8*2] = (a4-a6 +s2   + (1<<13))>>14;

    b[8*3] = (a0-a2+a7+a3 + (1<<13))>>14;



    b[8*4] = (a0-a2-a7-a3 + (1<<13))>>14;

    b[8*5] = (a4-a6 -s2   + (1<<13))>>14;

    b[8*6] = (a4+a6 -s1   + (1<<13))>>14;

    b[8*7] = (a0+a2-a1-a5 + (1<<13))>>14;

}