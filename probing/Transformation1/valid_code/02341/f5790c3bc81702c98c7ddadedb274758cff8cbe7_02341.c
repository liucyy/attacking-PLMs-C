uint64_t helper_cmpbge(uint64_t op1, uint64_t op2)

{

#if defined(__SSE2__)

    uint64_t r;



    

    {

        typedef uint64_t Q __attribute__((vector_size(16)));

        typedef uint8_t B __attribute__((vector_size(16)));



        Q q1 = (Q){ op1, 0 };

        Q q2 = (Q){ op2, 0 };



        q1 = (Q)((B)q1 >= (B)q2);



        r = q1[0];

    }



    

    r &= 0x0101010101010101;



    

    

    r |= r >> (8 - 1);



    

    r |= r >> (16 - 2);



    

    r |= r >> (32 - 4);



    

    

    return r & 0xff;

#else

    uint8_t opa, opb, res;

    int i;



    res = 0;

    for (i = 0; i < 8; i++) {

        opa = op1 >> (i * 8);

        opb = op2 >> (i * 8);

        if (opa >= opb) {

            res |= 1 << i;

        }

    }

    return res;

#endif

}