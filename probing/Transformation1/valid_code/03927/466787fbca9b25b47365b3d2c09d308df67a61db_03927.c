static void bdrv_replace_child(BdrvChild *child, BlockDriverState *new_bs,

                               bool check_new_perm)

{

    BlockDriverState *old_bs = child->bs;

    uint64_t perm, shared_perm;



    if (old_bs) {

        

        bdrv_get_cumulative_perm(old_bs, &perm, &shared_perm);

        bdrv_check_perm(old_bs, perm, shared_perm, NULL, &error_abort);

        bdrv_set_perm(old_bs, perm, shared_perm);

    }



    bdrv_replace_child_noperm(child, new_bs);



    if (new_bs) {

        bdrv_get_cumulative_perm(new_bs, &perm, &shared_perm);

        if (check_new_perm) {

            bdrv_check_perm(new_bs, perm, shared_perm, NULL, &error_abort);

        }

        bdrv_set_perm(new_bs, perm, shared_perm);

    }

}