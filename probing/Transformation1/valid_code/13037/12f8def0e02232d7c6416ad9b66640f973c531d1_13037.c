void qemu_mutex_lock(QemuMutex *mutex)

{

    EnterCriticalSection(&mutex->lock);



    

    assert(mutex->owner == 0);

    mutex->owner = GetCurrentThreadId();

}