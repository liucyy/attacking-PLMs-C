void qemu_thread_get_self(QemuThread *thread)

{

    if (!thread->thread) {

        

        qemu_thread_init();

        TlsSetValue(qemu_thread_tls_index, thread);

        thread->thread = GetCurrentThread();

    }

}