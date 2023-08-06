static void test_visitor_in_int_overflow(TestInputVisitorData *data,

                                         const void *unused)

{

    int64_t res = 0;

    Error *err = NULL;

    Visitor *v;



    

    v = visitor_input_test_init(data, "%f", DBL_MAX);



    visit_type_int(v, &res, NULL, &err);

    g_assert(err);

    error_free(err);

}