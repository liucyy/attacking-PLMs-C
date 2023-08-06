void qemu_cond_init(QemuCond *cond)

{

    memset(cond, 0, sizeof(*cond));



    cond->sema = CreateSemaphore(NULL, 0, LONG_MAX, NULL);

    if (!cond->sema) {

        error_exit(GetLastError(), __func__);

    }

    cond->continue_event = CreateEvent(NULL,    

                                       FALSE,   

                                       FALSE,   

                                       NULL);   

    if (!cond->continue_event) {

        error_exit(GetLastError(), __func__);

    }

}