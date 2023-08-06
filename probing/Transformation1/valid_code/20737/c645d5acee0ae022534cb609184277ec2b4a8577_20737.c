static void test_dummy_createcmdl(void)

{

    QemuOpts *opts;

    DummyObject *dobj;

    Error *err = NULL;

    const char *params = TYPE_DUMMY \

                         ",id=dev0," \

                         "bv=yes,sv=Hiss hiss hiss,av=platypus";



    qemu_add_opts(&qemu_object_opts);

    opts = qemu_opts_parse(&qemu_object_opts, params, true, &err);

    g_assert(err == NULL);

    g_assert(opts);



    dobj = DUMMY_OBJECT(user_creatable_add_opts(opts, &err));

    g_assert(err == NULL);

    g_assert(dobj);

    g_assert_cmpstr(dobj->sv, ==, "Hiss hiss hiss");

    g_assert(dobj->bv == true);

    g_assert(dobj->av == DUMMY_PLATYPUS);



    user_creatable_del("dev0", &err);

    g_assert(err == NULL);

    error_free(err);



    

}