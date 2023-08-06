Object *object_dynamic_cast(Object *obj, const char *typename)

{

    GSList *i;



    

    if (object_is_type(obj, typename)) {

        return obj;

    }



    

    for (i = obj->interfaces; i; i = i->next) {

        Interface *iface = i->data;



        if (object_is_type(OBJECT(iface), typename)) {

            return OBJECT(iface);

        }

    }



    

    if (object_is_type(obj, TYPE_INTERFACE)) {

        Interface *iface = INTERFACE(obj);



        if (object_is_type(iface->obj, typename)) {

            return iface->obj;

        }

    }



    return NULL;

}