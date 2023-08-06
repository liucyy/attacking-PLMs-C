static int loadvm_postcopy_handle_run(MigrationIncomingState *mis)

{

    PostcopyState ps = postcopy_state_set(POSTCOPY_INCOMING_RUNNING);



    trace_loadvm_postcopy_handle_run();

    if (ps != POSTCOPY_INCOMING_LISTENING) {

        error_report("CMD_POSTCOPY_RUN in wrong postcopy state (%d)", ps);

        return -1;

    }



    mis->bh = qemu_bh_new(loadvm_postcopy_handle_run_bh, NULL);

    qemu_bh_schedule(mis->bh);



    

    return LOADVM_QUIT;

}