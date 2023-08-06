static void iothread_complete(UserCreatable *obj, Error **errp)

{

    Error *local_error = NULL;

    IOThread *iothread = IOTHREAD(obj);



    iothread->stopping = false;

    iothread->thread_id = -1;

    iothread->ctx = aio_context_new(&local_error);

    if (!iothread->ctx) {

        error_propagate(errp, local_error);

        return;

    }



    qemu_mutex_init(&iothread->init_done_lock);

    qemu_cond_init(&iothread->init_done_cond);



    

    qemu_thread_create(&iothread->thread, "iothread", iothread_run,

                       iothread, QEMU_THREAD_JOINABLE);



    

    qemu_mutex_lock(&iothread->init_done_lock);

    while (iothread->thread_id == -1) {

        qemu_cond_wait(&iothread->init_done_cond,

                       &iothread->init_done_lock);

    }

    qemu_mutex_unlock(&iothread->init_done_lock);

}