static void ide_dev_set_bootindex(Object *obj, Visitor *v, const char *name,

                                  void *opaque, Error **errp)

{

    IDEDevice *d = IDE_DEVICE(obj);

    int32_t boot_index;

    Error *local_err = NULL;



    visit_type_int32(v, name, &boot_index, &local_err);

    if (local_err) {

        goto out;

    }

    

    check_boot_index(boot_index, &local_err);

    if (local_err) {

        goto out;

    }

    

    d->conf.bootindex = boot_index;



    if (d->unit != -1) {

        add_boot_device_path(d->conf.bootindex, &d->qdev,

                             d->unit ? "/disk@1" : "/disk@0");

    }

out:

    if (local_err) {

        error_propagate(errp, local_err);

    }

}