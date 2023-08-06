static void test_qemu_strtosz_simple(void)

{

    const char *str;

    char *endptr = NULL;

    int64_t res;



    str = "0";

    res = qemu_strtosz(str, &endptr);

    g_assert_cmpint(res, ==, 0);

    g_assert(endptr == str + 1);



    str = "12345";

    res = qemu_strtosz(str, &endptr);

    g_assert_cmpint(res, ==, 12345);

    g_assert(endptr == str + 5);



    res = qemu_strtosz(str, NULL);

    g_assert_cmpint(res, ==, 12345);



    



    str = "9007199254740991"; 

    res = qemu_strtosz(str, &endptr);

    g_assert_cmpint(res, ==, 0x1fffffffffffff);

    g_assert(endptr == str + 16);



    str = "9007199254740992"; 

    res = qemu_strtosz(str, &endptr);

    g_assert_cmpint(res, ==, 0x20000000000000);

    g_assert(endptr == str + 16);



    str = "9007199254740993"; 

    res = qemu_strtosz(str, &endptr);

    g_assert_cmpint(res, ==, 0x20000000000000); 

    g_assert(endptr == str + 16);



    str = "9223372036854774784"; 

    res = qemu_strtosz(str, &endptr);

    g_assert_cmpint(res, ==, 0x7ffffffffffffc00);

    g_assert(endptr == str + 19);



    str = "9223372036854775295"; 

    res = qemu_strtosz(str, &endptr);

    g_assert_cmpint(res, ==, 0x7ffffffffffffc00); 

    g_assert(endptr == str + 19);



    

}