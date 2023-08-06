static void test_visitor_in_null(TestInputVisitorData *data,

                                 const void *unused)

{

    Visitor *v;

    Error *err = NULL;

    char *tmp;



    



    v = visitor_input_test_init(data, "{ 'a': null, 'b': '', 'c': null }");

    visit_start_struct(v, NULL, NULL, 0, &error_abort);

    visit_type_null(v, "a", &error_abort);

    visit_type_null(v, "b", &err);

    error_free_or_abort(&err);

    visit_type_str(v, "c", &tmp, &err);

    g_assert(!tmp);

    error_free_or_abort(&err);

    visit_check_struct(v, &error_abort);

    visit_end_struct(v, NULL);

}