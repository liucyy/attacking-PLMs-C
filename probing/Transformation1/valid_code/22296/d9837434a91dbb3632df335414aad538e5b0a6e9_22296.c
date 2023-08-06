static inline int decode_scalar(GetBitContext *gb, int k, int limit, int readsamplesize){

    int x = get_unary_0_9(gb);



    if (x > 8) { 

        

        x = get_bits(gb, readsamplesize);

    } else {

        if (k >= limit)

            k = limit;



        if (k != 1) {

            int extrabits = show_bits(gb, k);



            

            x = (x << k) - x;



            if (extrabits > 1) {

                x += extrabits - 1;

                skip_bits(gb, k);

            } else

                skip_bits(gb, k - 1);

        }

    }

    return x;

}