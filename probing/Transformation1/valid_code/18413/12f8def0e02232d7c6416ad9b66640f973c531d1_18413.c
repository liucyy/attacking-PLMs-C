void qemu_cond_signal(QemuCond *cond)

{

    DWORD result;



    

    if (cond->waiters == 0) {

        return;

    }



    

    cond->target = cond->waiters - 1;

    result = SignalObjectAndWait(cond->sema, cond->continue_event,

                                 INFINITE, FALSE);

    if (result == WAIT_ABANDONED || result == WAIT_FAILED) {

        error_exit(GetLastError(), __func__);

    }

}