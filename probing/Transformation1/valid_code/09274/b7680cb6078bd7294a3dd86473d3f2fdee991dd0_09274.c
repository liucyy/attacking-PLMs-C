void vm_stop(int reason)

{

    QemuThread me;

    qemu_thread_self(&me);



    if (!qemu_thread_equal(&me, &io_thread)) {

        qemu_system_vmstop_request(reason);

        

        cpu_stop_current();

        return;

    }

    do_vm_stop(reason);

}