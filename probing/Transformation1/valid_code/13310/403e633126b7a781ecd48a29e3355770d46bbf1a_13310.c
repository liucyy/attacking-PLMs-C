static unsigned __stdcall win32_start_routine(void *arg)

{

    struct QemuThreadData data = *(struct QemuThreadData *) arg;

    QemuThread *thread = data.thread;



    free(arg);

    TlsSetValue(qemu_thread_tls_index, thread);



    

    DuplicateHandle(GetCurrentProcess(), GetCurrentThread(),

                    GetCurrentProcess(), &thread->thread,

                    0, FALSE, DUPLICATE_SAME_ACCESS);



    qemu_thread_exit(data.start_routine(data.arg));

    abort();

}