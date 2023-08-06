static void qapi_clone_type_str(Visitor *v, const char *name, char **obj,

                                 Error **errp)

{

    QapiCloneVisitor *qcv = to_qcv(v);



    assert(qcv->depth);

    

    *obj = g_strdup(*obj ?: "");

}