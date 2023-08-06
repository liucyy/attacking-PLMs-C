void vm_start(void)

{

    RunState requested;



    qemu_vmstop_requested(&requested);

    if (runstate_is_running() && requested == RUN_STATE__MAX) {

        return;

    }



    

    if (runstate_is_running()) {

        qapi_event_send_stop(&error_abort);

    } else {


        cpu_enable_ticks();

        runstate_set(RUN_STATE_RUNNING);

        vm_state_notify(1, RUN_STATE_RUNNING);

        resume_all_vcpus();

    }



    qapi_event_send_resume(&error_abort);

}