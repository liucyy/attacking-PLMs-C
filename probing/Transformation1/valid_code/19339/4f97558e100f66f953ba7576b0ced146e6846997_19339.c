void *colo_process_incoming_thread(void *opaque)

{

    MigrationIncomingState *mis = opaque;



    migrate_set_state(&mis->state, MIGRATION_STATUS_ACTIVE,

                      MIGRATION_STATUS_COLO);



    mis->to_src_file = qemu_file_get_return_path(mis->from_src_file);

    if (!mis->to_src_file) {

        error_report("COLO incoming thread: Open QEMUFile to_src_file failed");

        goto out;

    }

    

    qemu_file_set_blocking(mis->from_src_file, true);



    



out:

    if (mis->to_src_file) {

        qemu_fclose(mis->to_src_file);

    }

    migration_incoming_exit_colo();



    return NULL;

}