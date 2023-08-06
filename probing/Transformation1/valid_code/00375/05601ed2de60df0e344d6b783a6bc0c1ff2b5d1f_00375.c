void object_property_set_qobject(Object *obj, QObject *value,

                                 const char *name, Error **errp)

{

    Visitor *v;

    

    v = qobject_input_visitor_new(value, false);

    object_property_set(obj, v, name, errp);

    visit_free(v);

}