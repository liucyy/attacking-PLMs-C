static void test_lifecycle(void)

{

    Coroutine *coroutine;

    bool done = false;



    

    coroutine = qemu_coroutine_create(set_and_exit);

    qemu_coroutine_enter(coroutine, &done);

    g_assert(done); 



    

    done = false;

    coroutine = qemu_coroutine_create(set_and_exit);

    qemu_coroutine_enter(coroutine, &done);

    g_assert(done); 

}