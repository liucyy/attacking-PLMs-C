void bdrv_swap(BlockDriverState *bs_new, BlockDriverState *bs_old)

{

    BlockDriverState tmp;



    

    if (bs_new->node_name[0] != '\0') {

        QTAILQ_REMOVE(&graph_bdrv_states, bs_new, node_list);

    }

    if (bs_old->node_name[0] != '\0') {

        QTAILQ_REMOVE(&graph_bdrv_states, bs_old, node_list);

    }



    

    assert(bs_new->device_name[0] == '\0');

    assert(QLIST_EMPTY(&bs_new->dirty_bitmaps));

    assert(bs_new->job == NULL);

    assert(bs_new->dev == NULL);

    assert(bdrv_op_blocker_is_empty(bs_new));

    assert(bs_new->io_limits_enabled == false);

    assert(!throttle_have_timer(&bs_new->throttle_state));



    tmp = *bs_new;

    *bs_new = *bs_old;

    *bs_old = tmp;



    

    bdrv_move_feature_fields(&tmp, bs_old);

    bdrv_move_feature_fields(bs_old, bs_new);

    bdrv_move_feature_fields(bs_new, &tmp);



    

    assert(bs_new->device_name[0] == '\0');



    

    assert(bs_new->dev == NULL);

    assert(bs_new->job == NULL);

    assert(bdrv_op_blocker_is_empty(bs_new));

    assert(bs_new->io_limits_enabled == false);

    assert(!throttle_have_timer(&bs_new->throttle_state));



    

    if (bs_new->node_name[0] != '\0') {

        QTAILQ_INSERT_TAIL(&graph_bdrv_states, bs_new, node_list);

    }

    if (bs_old->node_name[0] != '\0') {

        QTAILQ_INSERT_TAIL(&graph_bdrv_states, bs_old, node_list);

    }



    bdrv_rebind(bs_new);

    bdrv_rebind(bs_old);

}