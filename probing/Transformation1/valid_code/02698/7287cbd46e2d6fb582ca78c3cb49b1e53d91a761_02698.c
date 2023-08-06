static int postcopy_start(MigrationState *ms, bool *old_vm_running)

{

    int ret;

    QIOChannelBuffer *bioc;

    QEMUFile *fb;

    int64_t time_at_stop = qemu_clock_get_ms(QEMU_CLOCK_REALTIME);

    bool restart_block = false;

    int cur_state = MIGRATION_STATUS_ACTIVE;

    if (!migrate_pause_before_switchover()) {

        migrate_set_state(&ms->state, MIGRATION_STATUS_ACTIVE,

                          MIGRATION_STATUS_POSTCOPY_ACTIVE);

    }



    trace_postcopy_start();

    qemu_mutex_lock_iothread();

    trace_postcopy_start_set_run();



    qemu_system_wakeup_request(QEMU_WAKEUP_REASON_OTHER);

    *old_vm_running = runstate_is_running();

    global_state_store();

    ret = vm_stop_force_state(RUN_STATE_FINISH_MIGRATE);

    if (ret < 0) {

        goto fail;

    }



    ret = migration_maybe_pause(ms, &cur_state,

                                MIGRATION_STATUS_POSTCOPY_ACTIVE);

    if (ret < 0) {

        goto fail;

    }



    ret = bdrv_inactivate_all();

    if (ret < 0) {

        goto fail;

    }

    restart_block = true;



    

    qemu_savevm_state_complete_precopy(ms->to_dst_file, true, false);



    

    if (migrate_postcopy_ram()) {

        if (ram_postcopy_send_discard_bitmap(ms)) {

            error_report("postcopy send discard bitmap failed");

            goto fail;

        }

    }



    

    qemu_file_set_rate_limit(ms->to_dst_file, INT64_MAX);

    if (migrate_postcopy_ram()) {

        

        qemu_savevm_send_ping(ms->to_dst_file, 2);

    }



    

    bioc = qio_channel_buffer_new(4096);

    qio_channel_set_name(QIO_CHANNEL(bioc), "migration-postcopy-buffer");

    fb = qemu_fopen_channel_output(QIO_CHANNEL(bioc));

    object_unref(OBJECT(bioc));



    

    qemu_savevm_send_postcopy_listen(fb);



    qemu_savevm_state_complete_precopy(fb, false, false);

    if (migrate_postcopy_ram()) {

        qemu_savevm_send_ping(fb, 3);

    }



    qemu_savevm_send_postcopy_run(fb);



    



    

    ret = qemu_file_get_error(ms->to_dst_file);

    if (ret) {

        error_report("postcopy_start: Migration stream errored (pre package)");

        goto fail_closefb;

    }



    restart_block = false;



    

    if (qemu_savevm_send_packaged(ms->to_dst_file, bioc->data, bioc->usage)) {

        goto fail_closefb;

    }

    qemu_fclose(fb);



    

    ms->postcopy_after_devices = true;

    notifier_list_notify(&migration_state_notifiers, ms);



    ms->downtime =  qemu_clock_get_ms(QEMU_CLOCK_REALTIME) - time_at_stop;



    qemu_mutex_unlock_iothread();



    if (migrate_postcopy_ram()) {

        

        qemu_savevm_send_ping(ms->to_dst_file, 4);

    }



    if (migrate_release_ram()) {

        ram_postcopy_migrated_memory_release(ms);

    }



    ret = qemu_file_get_error(ms->to_dst_file);

    if (ret) {

        error_report("postcopy_start: Migration stream errored");

        migrate_set_state(&ms->state, MIGRATION_STATUS_POSTCOPY_ACTIVE,

                              MIGRATION_STATUS_FAILED);

    }



    return ret;



fail_closefb:

    qemu_fclose(fb);

fail:

    migrate_set_state(&ms->state, MIGRATION_STATUS_POSTCOPY_ACTIVE,

                          MIGRATION_STATUS_FAILED);

    if (restart_block) {

        

        Error *local_err = NULL;



        bdrv_invalidate_cache_all(&local_err);

        if (local_err) {

            error_report_err(local_err);

        }

    }

    qemu_mutex_unlock_iothread();

    return -1;

}