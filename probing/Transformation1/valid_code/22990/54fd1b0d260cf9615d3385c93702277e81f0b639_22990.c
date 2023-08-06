int bdrv_reopen_prepare(BDRVReopenState *reopen_state, BlockReopenQueue *queue,

                        Error **errp)

{

    int ret = -1;

    Error *local_err = NULL;

    BlockDriver *drv;

    QemuOpts *opts;

    const char *value;

    bool read_only;



    assert(reopen_state != NULL);

    assert(reopen_state->bs->drv != NULL);

    drv = reopen_state->bs->drv;



    

    opts = qemu_opts_create(&bdrv_runtime_opts, NULL, 0, &error_abort);

    qemu_opts_absorb_qdict(opts, reopen_state->options, &local_err);

    if (local_err) {

        error_propagate(errp, local_err);

        ret = -EINVAL;

        goto error;

    }



    update_flags_from_options(&reopen_state->flags, opts);



    

    value = qemu_opt_get(opts, "node-name");

    if (value) {

        qdict_put_str(reopen_state->options, "node-name", value);

    }



    value = qemu_opt_get(opts, "driver");

    if (value) {

        qdict_put_str(reopen_state->options, "driver", value);

    }



    

    read_only = !(reopen_state->flags & BDRV_O_RDWR);

    ret = bdrv_can_set_read_only(reopen_state->bs, read_only, true, &local_err);

    if (local_err) {

        error_propagate(errp, local_err);

        goto error;

    }



    

    bdrv_reopen_perm(queue, reopen_state->bs,

                     &reopen_state->perm, &reopen_state->shared_perm);



    ret = bdrv_flush(reopen_state->bs);

    if (ret) {

        error_setg_errno(errp, -ret, "Error flushing drive");

        goto error;

    }



    if (drv->bdrv_reopen_prepare) {

        ret = drv->bdrv_reopen_prepare(reopen_state, queue, &local_err);

        if (ret) {

            if (local_err != NULL) {

                error_propagate(errp, local_err);

            } else {

                error_setg(errp, "failed while preparing to reopen image '%s'",

                           reopen_state->bs->filename);

            }

            goto error;

        }

    } else {

        

        error_setg(errp, "Block format '%s' used by node '%s' "

                   "does not support reopening files", drv->format_name,

                   bdrv_get_device_or_node_name(reopen_state->bs));

        ret = -1;

        goto error;

    }



    

    if (qdict_size(reopen_state->options)) {

        const QDictEntry *entry = qdict_first(reopen_state->options);



        do {

            QString *new_obj = qobject_to_qstring(entry->value);

            const char *new = qstring_get_str(new_obj);

            

            const char *old = qdict_get_try_str(reopen_state->bs->options,

                                                entry->key);



            if (!old || strcmp(new, old)) {

                error_setg(errp, "Cannot change the option '%s'", entry->key);

                ret = -EINVAL;

                goto error;

            }

        } while ((entry = qdict_next(reopen_state->options, entry)));

    }



    ret = bdrv_check_perm(reopen_state->bs, queue, reopen_state->perm,

                          reopen_state->shared_perm, NULL, errp);

    if (ret < 0) {

        goto error;

    }



    ret = 0;



error:

    qemu_opts_del(opts);

    return ret;

}