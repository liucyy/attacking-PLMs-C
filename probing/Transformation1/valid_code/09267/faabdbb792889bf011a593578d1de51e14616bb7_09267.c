void qdev_property_add_static(DeviceState *dev, Property *prop,

                              Error **errp)

{

    Error *local_err = NULL;

    Object *obj = OBJECT(dev);



    

    if (!prop->info->get && !prop->info->set) {

        return;

    }



    object_property_add(obj, prop->name, prop->info->name,

                        prop->info->get, prop->info->set,

                        prop->info->release,

                        prop, &local_err);



    if (local_err) {

        error_propagate(errp, local_err);

        return;

    }



    object_property_set_description(obj, prop->name,

                                    prop->info->description,

                                    &error_abort);



    if (prop->info->set_default_value) {

        prop->info->set_default_value(obj, prop);

    }

}