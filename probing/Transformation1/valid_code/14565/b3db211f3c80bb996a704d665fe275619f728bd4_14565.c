static void test_visitor_in_native_list_string(TestInputVisitorData *data,

                                               const void *unused)

{

    UserDefNativeListUnion *cvalue = NULL;

    strList *elem = NULL;

    Visitor *v;

    GString *gstr_list = g_string_new("");

    GString *gstr_union = g_string_new("");

    int i;



    for (i = 0; i < 32; i++) {

        g_string_append_printf(gstr_list, "'%d'", i);

        if (i != 31) {

            g_string_append(gstr_list, ", ");

        }

    }

    g_string_append_printf(gstr_union,  "{ 'type': 'string', 'data': [ %s ] }",

                           gstr_list->str);

    v = visitor_input_test_init_raw(data,  gstr_union->str);



    visit_type_UserDefNativeListUnion(v, NULL, &cvalue, &error_abort);

    g_assert(cvalue != NULL);

    g_assert_cmpint(cvalue->type, ==, USER_DEF_NATIVE_LIST_UNION_KIND_STRING);



    for (i = 0, elem = cvalue->u.string.data; elem; elem = elem->next, i++) {

        gchar str[8];

        sprintf(str, "%d", i);

        g_assert_cmpstr(elem->value, ==, str);

    }



    g_string_free(gstr_union, true);

    g_string_free(gstr_list, true);

    qapi_free_UserDefNativeListUnion(cvalue);

}