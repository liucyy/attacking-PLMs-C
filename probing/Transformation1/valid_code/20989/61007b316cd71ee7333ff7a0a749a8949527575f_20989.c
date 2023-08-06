int bdrv_is_allocated_above(BlockDriverState *top,

                            BlockDriverState *base,

                            int64_t sector_num,

                            int nb_sectors, int *pnum)

{

    BlockDriverState *intermediate;

    int ret, n = nb_sectors;



    intermediate = top;

    while (intermediate && intermediate != base) {

        int pnum_inter;

        ret = bdrv_is_allocated(intermediate, sector_num, nb_sectors,

                                &pnum_inter);

        if (ret < 0) {

            return ret;

        } else if (ret) {

            *pnum = pnum_inter;

            return 1;

        }



        

        if (n > pnum_inter &&

            (intermediate == top ||

             sector_num + pnum_inter < intermediate->total_sectors)) {

            n = pnum_inter;

        }



        intermediate = intermediate->backing_hd;

    }



    *pnum = n;

    return 0;

}