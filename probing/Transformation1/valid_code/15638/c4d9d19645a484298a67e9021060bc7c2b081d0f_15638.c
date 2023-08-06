static void test_cancel(void)

{

    WorkerTestData data[100];

    int num_canceled;

    int i;



    

    test_submit_many();



    

    for (i = 0; i < 100; i++) {

        data[i].n = 0;

        data[i].ret = -EINPROGRESS;

        data[i].aiocb = thread_pool_submit_aio(long_cb, &data[i],

                                               done_cb, &data[i]);

    }



    

    active = 100;

    qemu_aio_wait_nonblocking();



    

    g_assert_cmpint(active, ==, 100);

    g_usleep(1000000);

    g_assert_cmpint(active, >, 50);



    

    num_canceled = 0;

    for (i = 0; i < 100; i++) {

        if (__sync_val_compare_and_swap(&data[i].n, 0, 3) == 0) {

            data[i].ret = -ECANCELED;

            bdrv_aio_cancel(data[i].aiocb);

            active--;

            num_canceled++;

        }

    }

    g_assert_cmpint(active, >, 0);

    g_assert_cmpint(num_canceled, <, 100);



    

    for (i = 0; i < 100; i++) {

        if (data[i].n != 3) {

            bdrv_aio_cancel(data[i].aiocb);

        }

    }



    

    qemu_aio_wait_all();

    g_assert_cmpint(active, ==, 0);

    for (i = 0; i < 100; i++) {

        if (data[i].n == 3) {

            g_assert_cmpint(data[i].ret, ==, -ECANCELED);

            g_assert(data[i].aiocb != NULL);

        } else {

            g_assert_cmpint(data[i].n, ==, 2);

            g_assert_cmpint(data[i].ret, ==, 0);

            g_assert(data[i].aiocb == NULL);

        }

    }

}