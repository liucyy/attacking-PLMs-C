static void test_ivshmem_pair(void)

{

    IVState state1, state2, *s1, *s2;

    char *data;

    int i;



    setup_vm(&state1);

    s1 = &state1;

    setup_vm(&state2);

    s2 = &state2;



    data = g_malloc0(TMPSHMSIZE);



    

    memset(tmpshmem, 0x42, TMPSHMSIZE);

    qtest_memread(s1->qtest, (uintptr_t)s1->mem_base, data, TMPSHMSIZE);

    for (i = 0; i < TMPSHMSIZE; i++) {

        g_assert_cmpuint(data[i], ==, 0x42);

    }

    qtest_memread(s2->qtest, (uintptr_t)s2->mem_base, data, TMPSHMSIZE);

    for (i = 0; i < TMPSHMSIZE; i++) {

        g_assert_cmpuint(data[i], ==, 0x42);

    }



    

    memset(data, 0x43, TMPSHMSIZE);

    qtest_memwrite(s1->qtest, (uintptr_t)s1->mem_base, data, TMPSHMSIZE);

    memset(data, 0, TMPSHMSIZE);

    qtest_memread(s2->qtest, (uintptr_t)s2->mem_base, data, TMPSHMSIZE);

    for (i = 0; i < TMPSHMSIZE; i++) {

        g_assert_cmpuint(data[i], ==, 0x43);

    }



    

    memset(data, 0x44, TMPSHMSIZE);

    qtest_memwrite(s2->qtest, (uintptr_t)s2->mem_base, data, TMPSHMSIZE);

    memset(data, 0, TMPSHMSIZE);

    qtest_memread(s1->qtest, (uintptr_t)s2->mem_base, data, TMPSHMSIZE);

    for (i = 0; i < TMPSHMSIZE; i++) {

        g_assert_cmpuint(data[i], ==, 0x44);

    }



    qtest_quit(s1->qtest);

    qtest_quit(s2->qtest);

    g_free(data);

}