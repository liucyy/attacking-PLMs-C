QObject *qmp_output_get_qobject(QmpOutputVisitor *qov)

{

    

    QObject *obj = qov->root;

    if (obj) {

        qobject_incref(obj);

    } else {

        obj = qnull();

    }

    return obj;

}