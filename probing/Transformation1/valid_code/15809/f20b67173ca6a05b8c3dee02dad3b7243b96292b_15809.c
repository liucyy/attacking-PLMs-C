static inline int coef_test_compression(int coef)

{

    int tmp = coef >> 2;

    int res = ff_ctz(tmp);

    if (res > 1)

        return 1;       

    else if (res == 1)

        return 0;       

    else if (ff_ctz(tmp >> 1) > 0)

        return 0;       

    else

        return 1;       

}