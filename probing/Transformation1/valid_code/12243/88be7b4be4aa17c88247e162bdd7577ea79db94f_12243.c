bool bdrv_is_first_non_filter(BlockDriverState *candidate)

{

    BlockDriverState *bs;

    BdrvNextIterator *it = NULL;



    

    while ((it = bdrv_next(it, &bs)) != NULL) {

        bool perm;



        

        perm = bdrv_recurse_is_first_non_filter(bs, candidate);



        

        if (perm) {

            return true;

        }

    }



    return false;

}