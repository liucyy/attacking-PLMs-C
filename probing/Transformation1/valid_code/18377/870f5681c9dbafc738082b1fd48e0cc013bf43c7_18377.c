void qmp_transaction(BlockdevActionList *dev_list, Error **errp)

{

    int ret = 0;

    BlockdevActionList *dev_entry = dev_list;

    BlkTransactionStates *states, *next;



    QSIMPLEQ_HEAD(snap_bdrv_states, BlkTransactionStates) snap_bdrv_states;

    QSIMPLEQ_INIT(&snap_bdrv_states);



    

    bdrv_drain_all();



    

    while (NULL != dev_entry) {

        BlockdevAction *dev_info = NULL;

        BlockDriver *proto_drv;

        BlockDriver *drv;

        int flags;

        enum NewImageMode mode;

        const char *new_image_file;

        const char *device;

        const char *format = "qcow2";



        dev_info = dev_entry->value;

        dev_entry = dev_entry->next;



        states = g_malloc0(sizeof(BlkTransactionStates));

        QSIMPLEQ_INSERT_TAIL(&snap_bdrv_states, states, entry);



        switch (dev_info->kind) {

        case BLOCKDEV_ACTION_KIND_BLOCKDEV_SNAPSHOT_SYNC:

            device = dev_info->blockdev_snapshot_sync->device;

            if (!dev_info->blockdev_snapshot_sync->has_mode) {

                dev_info->blockdev_snapshot_sync->mode = NEW_IMAGE_MODE_ABSOLUTE_PATHS;

            }

            new_image_file = dev_info->blockdev_snapshot_sync->snapshot_file;

            if (dev_info->blockdev_snapshot_sync->has_format) {

                format = dev_info->blockdev_snapshot_sync->format;

            }

            mode = dev_info->blockdev_snapshot_sync->mode;

            break;

        default:

            abort();

        }



        drv = bdrv_find_format(format);

        if (!drv) {

            error_set(errp, QERR_INVALID_BLOCK_FORMAT, format);

            goto delete_and_fail;

        }



        states->old_bs = bdrv_find(device);

        if (!states->old_bs) {

            error_set(errp, QERR_DEVICE_NOT_FOUND, device);

            goto delete_and_fail;

        }



        if (!bdrv_is_inserted(states->old_bs)) {

            error_set(errp, QERR_DEVICE_HAS_NO_MEDIUM, device);

            goto delete_and_fail;

        }



        if (bdrv_in_use(states->old_bs)) {

            error_set(errp, QERR_DEVICE_IN_USE, device);

            goto delete_and_fail;

        }



        if (!bdrv_is_read_only(states->old_bs)) {

            if (bdrv_flush(states->old_bs)) {

                error_set(errp, QERR_IO_ERROR);

                goto delete_and_fail;

            }

        }



        flags = states->old_bs->open_flags;



        proto_drv = bdrv_find_protocol(new_image_file);

        if (!proto_drv) {

            error_set(errp, QERR_INVALID_BLOCK_FORMAT, format);

            goto delete_and_fail;

        }



        

        if (mode != NEW_IMAGE_MODE_EXISTING) {

            ret = bdrv_img_create(new_image_file, format,

                                  states->old_bs->filename,

                                  states->old_bs->drv->format_name,

                                  NULL, -1, flags);

            if (ret) {

                error_set(errp, QERR_OPEN_FILE_FAILED, new_image_file);

                goto delete_and_fail;

            }

        }



        

        states->new_bs = bdrv_new("");

        ret = bdrv_open(states->new_bs, new_image_file,

                        flags | BDRV_O_NO_BACKING, drv);

        if (ret != 0) {

            error_set(errp, QERR_OPEN_FILE_FAILED, new_image_file);

            goto delete_and_fail;

        }

    }





    

    QSIMPLEQ_FOREACH(states, &snap_bdrv_states, entry) {

        

        bdrv_append(states->new_bs, states->old_bs);






    }



    

    goto exit;



delete_and_fail:

    

    QSIMPLEQ_FOREACH(states, &snap_bdrv_states, entry) {

        if (states->new_bs) {

             bdrv_delete(states->new_bs);

        }

    }

exit:

    QSIMPLEQ_FOREACH_SAFE(states, &snap_bdrv_states, entry, next) {

        g_free(states);

    }

    return;

}