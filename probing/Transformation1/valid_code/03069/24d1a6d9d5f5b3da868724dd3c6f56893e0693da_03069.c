static void test_wait_event_notifier_noflush(void)

{

    EventNotifierTestData data = { .n = 0 };

    EventNotifierTestData dummy = { .n = 0, .active = 1 };



    event_notifier_init(&data.e, false);

    aio_set_event_notifier(ctx, &data.e, event_ready_cb, NULL);



    g_assert(!aio_poll(ctx, false));

    g_assert_cmpint(data.n, ==, 0);



    

    event_notifier_set(&data.e);

    g_assert(!aio_poll(ctx, true));

    data.n = 0;



    

    event_notifier_init(&dummy.e, false);

    aio_set_event_notifier(ctx, &dummy.e, event_ready_cb, event_active_cb);



    event_notifier_set(&data.e);

    g_assert(aio_poll(ctx, false));

    g_assert_cmpint(data.n, ==, 1);

    g_assert(aio_poll(ctx, false));

    g_assert_cmpint(data.n, ==, 1);



    event_notifier_set(&data.e);

    g_assert(aio_poll(ctx, false));

    g_assert_cmpint(data.n, ==, 2);

    g_assert(aio_poll(ctx, false));

    g_assert_cmpint(data.n, ==, 2);



    event_notifier_set(&dummy.e);

    wait_for_aio();

    g_assert_cmpint(data.n, ==, 2);

    g_assert_cmpint(dummy.n, ==, 1);

    g_assert_cmpint(dummy.active, ==, 0);



    aio_set_event_notifier(ctx, &dummy.e, NULL, NULL);

    event_notifier_cleanup(&dummy.e);



    aio_set_event_notifier(ctx, &data.e, NULL, NULL);

    g_assert(!aio_poll(ctx, false));

    g_assert_cmpint(data.n, ==, 2);



    event_notifier_cleanup(&data.e);

}