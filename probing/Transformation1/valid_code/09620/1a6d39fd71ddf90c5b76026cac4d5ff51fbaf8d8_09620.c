static int is_not_zero(const uint8_t *sector, int len)

{

    



    int i;

    long d0, d1, d2, d3;

    const long * const data = (const long *) sector;



    len /= sizeof(long);



    for(i = 0; i < len; i += 4) {

        d0 = data[i + 0];

        d1 = data[i + 1];

        d2 = data[i + 2];

        d3 = data[i + 3];



        if (d0 || d1 || d2 || d3) {

            return 1;

        }

    }



    return 0;

}