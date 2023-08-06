void qemu_cond_wait(QemuCond *cond, QemuMutex *mutex)

{

    

    cond->waiters++;



    

    qemu_mutex_unlock(mutex);

    WaitForSingleObject(cond->sema, INFINITE);



    

    if (InterlockedDecrement(&cond->waiters) == cond->target) {

        SetEvent(cond->continue_event);

    }



    qemu_mutex_lock(mutex);

}