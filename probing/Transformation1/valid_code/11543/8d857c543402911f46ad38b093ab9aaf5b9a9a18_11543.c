static inline int get_block(GetBitContext *gb, DCTELEM *block, const uint8_t *scan,

                            const uint32_t *quant) {

    int coeff, i, n;

    int8_t ac;

    uint8_t dc = get_bits(gb, 8);



    

    if (dc == 255)




    

    coeff = get_bits(gb, 6);

    if (get_bits_count(gb) + (coeff << 1) >= gb->size_in_bits)




    

    

    memset(block, 0, 64 * sizeof(DCTELEM));



    

    while (coeff) {

        ac = get_sbits(gb, 2);

        if (ac == -2)

            break; 

        PUT_COEFF(ac);

    }



    

    ALIGN(4);



    while (coeff) {

        ac = get_sbits(gb, 4);

        if (ac == -8)

            break; 

        PUT_COEFF(ac);

    }



    

    ALIGN(8);

    if (get_bits_count(gb) + (coeff << 3) >= gb->size_in_bits)


    while (coeff) {

        ac = get_sbits(gb, 8);

        PUT_COEFF(ac);

    }



    PUT_COEFF(dc);

    return 1;

}