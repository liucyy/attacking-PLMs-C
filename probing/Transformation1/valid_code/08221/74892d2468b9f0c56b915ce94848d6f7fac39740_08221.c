int vm_stop(RunState state)

{

    if (qemu_in_vcpu_thread()) {


        qemu_system_vmstop_request(state);

        

        cpu_stop_current();

        return 0;

    }



    return do_vm_stop(state);

}