static int qdev_add_one_global(QemuOpts *opts, void *opaque)

{

    GlobalProperty *g;

    ObjectClass *oc;



    g = g_malloc0(sizeof(*g));

    g->driver   = qemu_opt_get(opts, "driver");

    g->property = qemu_opt_get(opts, "property");

    g->value    = qemu_opt_get(opts, "value");

    oc = object_class_by_name(g->driver);

    if (oc) {

        DeviceClass *dc = DEVICE_CLASS(oc);



        if (dc->hotpluggable) {

            

            g->not_used = false;

        } else {

            

            g->not_used = true;

        }

    } else {

        

        g->not_used = true;

    }

    qdev_prop_register_global(g);

    return 0;

}