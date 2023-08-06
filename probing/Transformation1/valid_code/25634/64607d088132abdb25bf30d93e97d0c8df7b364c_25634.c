Object *object_resolve_path_component(Object *parent, const gchar *part)

{

    ObjectProperty *prop = object_property_find(parent, part, NULL);

    if (prop == NULL) {

        return NULL;

    }



    if (object_property_is_link(prop)) {

        LinkProperty *lprop = prop->opaque;

        return *lprop->child;

    } else if (object_property_is_child(prop)) {

        return prop->opaque;

    } else {

        return NULL;

    }

}