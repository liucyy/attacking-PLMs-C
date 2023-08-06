static inline int get_symbol(RangeCoder *c, uint8_t *state, int is_signed){

    if(get_rac(c, state+0))

        return 0;

    else{

        int i, e, a;

        e= 0;

        while(get_rac(c, state+1 + e)){ 

            e++;

        }

        assert(e<=9);



        a= 1;

        for(i=e-1; i>=0; i--){

            a += a + get_rac(c, state+22 + i); 

        }



        if(is_signed && get_rac(c, state+11 + e)) 

            return -a;

        else

            return a;

    }

}