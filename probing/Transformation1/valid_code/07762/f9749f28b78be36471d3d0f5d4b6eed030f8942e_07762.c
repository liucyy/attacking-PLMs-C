static int coroutine_fn is_allocated_base(BlockDriverState *top,

                                          BlockDriverState *base,

                                          int64_t sector_num,

                                          int nb_sectors, int *pnum)

{

    BlockDriverState *intermediate;

    int ret, n;



    ret = bdrv_co_is_allocated(top, sector_num, nb_sectors, &n);

    if (ret) {

        *pnum = n;

        return ret;

    }



    

    intermediate = top->backing_hd;



    while (intermediate != base) {

        int pnum_inter;



        ret = bdrv_co_is_allocated(intermediate, sector_num, nb_sectors,

                                   &pnum_inter);

        if (ret < 0) {

            return ret;

        } else if (ret) {

            *pnum = pnum_inter;

            return 0;

        }



        

        if (n > pnum_inter) {

            n = pnum_inter;

        }



        intermediate = intermediate->backing_hd;

    }



    *pnum = n;

    return 1;

}