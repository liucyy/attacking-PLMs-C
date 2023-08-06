static void process_incoming_migration_bh(void *opaque)
{
    Error *local_err = NULL;
    MigrationIncomingState *mis = opaque;
    
    bdrv_invalidate_cache_all(&local_err);
        migrate_set_state(&mis->state, MIGRATION_STATUS_ACTIVE,
                          MIGRATION_STATUS_FAILED);
        error_report_err(local_err);
        migrate_decompress_threads_join();
        exit(EXIT_FAILURE);
    
    qemu_announce_self();
    
    if (!global_state_received() ||
        global_state_get_runstate() == RUN_STATE_RUNNING) {
        if (autostart) {
            vm_start();
        } else {
            runstate_set(RUN_STATE_PAUSED);
    } else {
        runstate_set(global_state_get_runstate());
    migrate_decompress_threads_join();
    
    migrate_set_state(&mis->state, MIGRATION_STATUS_ACTIVE,
                      MIGRATION_STATUS_COMPLETED);
    qemu_bh_delete(mis->bh);
    migration_incoming_state_destroy();