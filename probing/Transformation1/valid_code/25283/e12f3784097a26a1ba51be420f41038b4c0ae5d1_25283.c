BlockDriverState *check_to_replace_node(const char *node_name, Error **errp)

{

    BlockDriverState *to_replace_bs = bdrv_find_node(node_name);

    AioContext *aio_context;



    if (!to_replace_bs) {

        error_setg(errp, "Node name '%s' not found", node_name);

        return NULL;

    }



    aio_context = bdrv_get_aio_context(to_replace_bs);

    aio_context_acquire(aio_context);



    if (bdrv_op_is_blocked(to_replace_bs, BLOCK_OP_TYPE_REPLACE, errp)) {

        to_replace_bs = NULL;

        goto out;

    }



    

    if (!bdrv_is_first_non_filter(to_replace_bs)) {

        error_setg(errp, "Only top most non filter can be replaced");

        to_replace_bs = NULL;

        goto out;

    }



out:

    aio_context_release(aio_context);

    return to_replace_bs;

}