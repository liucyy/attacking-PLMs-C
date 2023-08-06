static void block_dirty_bitmap_add_abort(BlkActionState *common)

{

    BlockDirtyBitmapAdd *action;

    BlockDirtyBitmapState *state = DO_UPCAST(BlockDirtyBitmapState,

                                             common, common);



    action = common->action->u.block_dirty_bitmap_add;

    

    if (state->prepared) {

        qmp_block_dirty_bitmap_remove(action->node, action->name, &error_abort);

    }

}