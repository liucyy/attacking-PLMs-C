static void loadvm_postcopy_handle_run_bh(void *opaque)

{

    Error *local_err = NULL;

    HandleRunBhData *data = opaque;



    

    cpu_synchronize_all_post_init();



    qemu_announce_self();



    

    bdrv_invalidate_cache_all(&local_err);

    if (!local_err) {

        blk_resume_after_migration(&local_err);

    }

    if (local_err) {

        error_report_err(local_err);

        local_err = NULL;

        autostart = false;

    }



    trace_loadvm_postcopy_handle_run_cpu_sync();

    cpu_synchronize_all_post_init();



    trace_loadvm_postcopy_handle_run_vmstart();



    if (autostart) {

        

        vm_start();

    } else {

        

        runstate_set(RUN_STATE_PAUSED);

    }



    qemu_bh_delete(data->bh);

    g_free(data);

}