static bool object_is_type(Object *obj, const char *typename)

{

    TypeImpl *target_type = type_get_by_name(typename);

    TypeImpl *type = obj->class->type;

    GSList *i;



    

    while (type) {

        if (type == target_type) {

            return true;

        }



        type = type_get_parent(type);

    }



    

    for (i = obj->interfaces; i; i = i->next) {

        Interface *iface = i->data;



        if (object_is_type(OBJECT(iface), typename)) {

            return true;

        }

    }



    return false;

}