void *colo_process_incoming_thread(void *opaque)

{

    MigrationIncomingState *mis = opaque;

    QEMUFile *fb = NULL;

    QIOChannelBuffer *bioc = NULL; 

    uint64_t total_size;

    uint64_t value;

    Error *local_err = NULL;



    migrate_set_state(&mis->state, MIGRATION_STATUS_ACTIVE,

                      MIGRATION_STATUS_COLO);



    failover_init_state();



    mis->to_src_file = qemu_file_get_return_path(mis->from_src_file);

    if (!mis->to_src_file) {

        error_report("COLO incoming thread: Open QEMUFile to_src_file failed");

        goto out;

    }

    

    qemu_file_set_blocking(mis->from_src_file, true);



    bioc = qio_channel_buffer_new(COLO_BUFFER_BASE_SIZE);

    fb = qemu_fopen_channel_input(QIO_CHANNEL(bioc));

    object_unref(OBJECT(bioc));



    colo_send_message(mis->to_src_file, COLO_MESSAGE_CHECKPOINT_READY,

                      &local_err);

    if (local_err) {

        goto out;

    }



    while (mis->state == MIGRATION_STATUS_COLO) {

        int request;



        colo_wait_handle_message(mis->from_src_file, &request, &local_err);

        if (local_err) {

            goto out;

        }

        assert(request);

        if (failover_get_state() != FAILOVER_STATUS_NONE) {

            error_report("failover request");

            goto out;

        }



        

        colo_send_message(mis->to_src_file, COLO_MESSAGE_CHECKPOINT_REPLY,

                     &local_err);

        if (local_err) {

            goto out;

        }



        colo_receive_check_message(mis->from_src_file,

                           COLO_MESSAGE_VMSTATE_SEND, &local_err);

        if (local_err) {

            goto out;

        }



        value = colo_receive_message_value(mis->from_src_file,

                                 COLO_MESSAGE_VMSTATE_SIZE, &local_err);

        if (local_err) {

            goto out;

        }



        

        if (value > bioc->capacity) {

            bioc->capacity = value;

            bioc->data = g_realloc(bioc->data, bioc->capacity);

        }

        total_size = qemu_get_buffer(mis->from_src_file, bioc->data, value);

        if (total_size != value) {

            error_report("Got %" PRIu64 " VMState data, less than expected"

                        " %" PRIu64, total_size, value);

            goto out;

        }

        bioc->usage = total_size;

        qio_channel_io_seek(QIO_CHANNEL(bioc), 0, 0, NULL);



        colo_send_message(mis->to_src_file, COLO_MESSAGE_VMSTATE_RECEIVED,

                     &local_err);

        if (local_err) {

            goto out;

        }



        qemu_mutex_lock_iothread();

        qemu_system_reset(VMRESET_SILENT);

        if (qemu_loadvm_state(fb) < 0) {

            error_report("COLO: loadvm failed");

            qemu_mutex_unlock_iothread();

            goto out;

        }

        qemu_mutex_unlock_iothread();



        colo_send_message(mis->to_src_file, COLO_MESSAGE_VMSTATE_LOADED,

                     &local_err);

        if (local_err) {

            goto out;

        }

    }



out:

    

    if (local_err) {

        error_report_err(local_err);

    }



    if (fb) {

        qemu_fclose(fb);

    }



    if (mis->to_src_file) {

        qemu_fclose(mis->to_src_file);

    }

    migration_incoming_exit_colo();



    return NULL;

}