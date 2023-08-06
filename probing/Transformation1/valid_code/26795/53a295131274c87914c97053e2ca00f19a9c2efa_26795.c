int bdrv_open(BlockDriverState **pbs, const char *filename,

              const char *reference, QDict *options, int flags,

              BlockDriver *drv, Error **errp)

{

    int ret;

    BlockDriverState *file = NULL, *bs;

    const char *drvname;

    Error *local_err = NULL;

    int snapshot_flags = 0;



    assert(pbs);



    if (reference) {

        bool options_non_empty = options ? qdict_size(options) : false;

        QDECREF(options);



        if (*pbs) {

            error_setg(errp, "Cannot reuse an existing BDS when referencing "

                       "another block device");

            return -EINVAL;

        }



        if (filename || options_non_empty) {

            error_setg(errp, "Cannot reference an existing block device with "

                       "additional options or a new filename");

            return -EINVAL;

        }



        bs = bdrv_lookup_bs(reference, reference, errp);

        if (!bs) {

            return -ENODEV;

        }

        bdrv_ref(bs);

        *pbs = bs;

        return 0;

    }



    if (*pbs) {

        bs = *pbs;

    } else {

        bs = bdrv_new();

    }



    

    if (options == NULL) {

        options = qdict_new();

    }



    ret = bdrv_fill_options(&options, &filename, flags, drv, &local_err);

    if (local_err) {

        goto fail;

    }



    

    drv = NULL;

    drvname = qdict_get_try_str(options, "driver");

    if (drvname) {

        drv = bdrv_find_format(drvname);

        qdict_del(options, "driver");

        if (!drv) {

            error_setg(errp, "Unknown driver: '%s'", drvname);

            ret = -EINVAL;

            goto fail;

        }

    }



    assert(drvname || !(flags & BDRV_O_PROTOCOL));

    if (drv && !drv->bdrv_file_open) {

        

        flags &= ~BDRV_O_PROTOCOL;

    }



    bs->options = options;

    options = qdict_clone_shallow(options);



    

    if ((flags & BDRV_O_PROTOCOL) == 0) {

        if (flags & BDRV_O_RDWR) {

            flags |= BDRV_O_ALLOW_RDWR;

        }

        if (flags & BDRV_O_SNAPSHOT) {

            snapshot_flags = bdrv_temp_snapshot_flags(flags);

            flags = bdrv_backing_flags(flags);

        }



        assert(file == NULL);

        ret = bdrv_open_image(&file, filename, options, "file",

                              bdrv_inherited_flags(flags),

                              true, &local_err);

        if (ret < 0) {

            goto fail;

        }

    }



    

    bs->probed = !drv;

    if (!drv && file) {

        ret = find_image_format(file, filename, &drv, &local_err);

        if (ret < 0) {

            goto fail;

        }

    } else if (!drv) {

        error_setg(errp, "Must specify either driver or file");

        ret = -EINVAL;

        goto fail;

    }



    

    ret = bdrv_open_common(bs, file, options, flags, drv, &local_err);

    if (ret < 0) {

        goto fail;

    }



    if (file && (bs->file != file)) {

        bdrv_unref(file);

        file = NULL;

    }



    

    if ((flags & BDRV_O_NO_BACKING) == 0) {

        QDict *backing_options;



        qdict_extract_subqdict(options, &backing_options, "backing.");

        ret = bdrv_open_backing_file(bs, backing_options, &local_err);

        if (ret < 0) {

            goto close_and_fail;

        }

    }



    bdrv_refresh_filename(bs);



    

    if (snapshot_flags) {

        ret = bdrv_append_temp_snapshot(bs, snapshot_flags, &local_err);

        if (local_err) {

            goto close_and_fail;

        }

    }



    

    if (options && (qdict_size(options) != 0)) {

        const QDictEntry *entry = qdict_first(options);

        if (flags & BDRV_O_PROTOCOL) {

            error_setg(errp, "Block protocol '%s' doesn't support the option "

                       "'%s'", drv->format_name, entry->key);

        } else {

            error_setg(errp, "Block format '%s' used by device '%s' doesn't "

                       "support the option '%s'", drv->format_name,

                       bdrv_get_device_name(bs), entry->key);

        }



        ret = -EINVAL;

        goto close_and_fail;

    }



    if (!bdrv_key_required(bs)) {

        if (bs->blk) {

            blk_dev_change_media_cb(bs->blk, true);

        }

    } else if (!runstate_check(RUN_STATE_PRELAUNCH)

               && !runstate_check(RUN_STATE_INMIGRATE)

               && !runstate_check(RUN_STATE_PAUSED)) { 

        error_setg(errp,

                   "Guest must be stopped for opening of encrypted image");

        ret = -EBUSY;

        goto close_and_fail;

    }



    QDECREF(options);

    *pbs = bs;

    return 0;



fail:

    if (file != NULL) {

        bdrv_unref(file);

    }

    QDECREF(bs->options);

    QDECREF(options);

    bs->options = NULL;

    if (!*pbs) {

        

        bdrv_unref(bs);

    }

    if (local_err) {

        error_propagate(errp, local_err);

    }

    return ret;



close_and_fail:

    

    if (*pbs) {

        bdrv_close(bs);

    } else {

        bdrv_unref(bs);

    }

    QDECREF(options);

    if (local_err) {

        error_propagate(errp, local_err);

    }

    return ret;

}