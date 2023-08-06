void qemu_cond_broadcast(QemuCond *cond)

{

    BOOLEAN result;

    

    if (cond->waiters == 0) {

        return;

    }



    cond->target = 0;

    result = ReleaseSemaphore(cond->sema, cond->waiters, NULL);

    if (!result) {

        error_exit(GetLastError(), __func__);

    }



    

    WaitForSingleObject(cond->continue_event, INFINITE);

}