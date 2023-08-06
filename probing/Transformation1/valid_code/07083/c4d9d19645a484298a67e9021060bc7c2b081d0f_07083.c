static void co_test_cb(void *opaque)

{

    WorkerTestData *data = opaque;



    active = 1;

    data->n = 0;

    data->ret = -EINPROGRESS;

    thread_pool_submit_co(worker_cb, data);



    



    g_assert_cmpint(data->n, ==, 1);

    data->ret = 0;

    active--;



    

}