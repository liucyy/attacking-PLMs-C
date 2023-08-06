static void test_yield(void)

{

    Coroutine *coroutine;

    bool done = false;

    int i = -1; 



    coroutine = qemu_coroutine_create(yield_5_times);

    while (!done) {

        qemu_coroutine_enter(coroutine, &done);

        i++;

    }

    g_assert_cmpint(i, ==, 5); 

}