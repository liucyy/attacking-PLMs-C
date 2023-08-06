void *qemu_thread_join(QemuThread *thread)

{

    QemuThreadData *data;

    void *ret;

    HANDLE handle;



    data = thread->data;

    if (!data) {

        return NULL;

    }

    

    EnterCriticalSection(&data->cs);

    if (!data->exited) {

        handle = OpenThread(SYNCHRONIZE, FALSE, thread->tid);

        LeaveCriticalSection(&data->cs);

        WaitForSingleObject(handle, INFINITE);

        CloseHandle(handle);

    } else {

        LeaveCriticalSection(&data->cs);

    }

    ret = data->ret;

    DeleteCriticalSection(&data->cs);

    g_free(data);

    return ret;

}