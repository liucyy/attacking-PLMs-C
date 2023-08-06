static void nvme_set_bootindex(Object *obj, Visitor *v, void *opaque,

                                  const char *name, Error **errp)

{

    NvmeCtrl *s = NVME(obj);

    int32_t boot_index;

    Error *local_err = NULL;



    visit_type_int32(v, &boot_index, name, &local_err);

    if (local_err) {

        goto out;

    }

    

    check_boot_index(boot_index, &local_err);

    if (local_err) {

        goto out;

    }

    

    s->conf.bootindex = boot_index;



out:

    if (local_err) {

        error_propagate(errp, local_err);

    }

}