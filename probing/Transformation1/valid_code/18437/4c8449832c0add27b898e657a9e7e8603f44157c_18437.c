static void external_snapshot_commit(BlkActionState *common)

{

    ExternalSnapshotState *state =

                             DO_UPCAST(ExternalSnapshotState, common, common);



    bdrv_set_aio_context(state->new_bs, state->aio_context);



    

    bdrv_append(state->new_bs, state->old_bs);

    

    bdrv_reopen(state->old_bs, state->old_bs->open_flags & ~BDRV_O_RDWR,

                NULL);

}