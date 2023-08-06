static void blockdev_backup_abort(BlkActionState *common)

{

    BlockdevBackupState *state = DO_UPCAST(BlockdevBackupState, common, common);

    BlockDriverState *bs = state->bs;



    

    if (bs && bs->job && bs->job == state->job) {

        block_job_cancel_sync(bs->job);

    }

}