static void loadvm_postcopy_handle_run_bh(void *opaque)

{

    Error *local_err = NULL;

    MigrationIncomingState *mis = opaque;



    

    cpu_synchronize_all_post_init();



    qemu_announce_self();



    

    bdrv_invalidate_cache_all(&local_err);

    if (local_err) {

        error_report_err(local_err);

    }



    trace_loadvm_postcopy_handle_run_cpu_sync();

    cpu_synchronize_all_post_init();



    trace_loadvm_postcopy_handle_run_vmstart();



    if (autostart) {

        

        vm_start();

    } else {

        

        runstate_set(RUN_STATE_PAUSED);

    }



    qemu_bh_delete(mis->bh);

}