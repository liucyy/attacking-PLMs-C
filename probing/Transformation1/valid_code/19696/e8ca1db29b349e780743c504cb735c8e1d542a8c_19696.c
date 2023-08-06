static int loadvm_postcopy_handle_advise(MigrationIncomingState *mis)

{

    PostcopyState ps = postcopy_state_set(POSTCOPY_INCOMING_ADVISE);

    uint64_t remote_hps, remote_tps;



    trace_loadvm_postcopy_handle_advise();

    if (ps != POSTCOPY_INCOMING_NONE) {

        error_report("CMD_POSTCOPY_ADVISE in wrong postcopy state (%d)", ps);

        return -1;

    }



    if (!postcopy_ram_supported_by_host()) {

        postcopy_state_set(POSTCOPY_INCOMING_NONE);

        return -1;

    }



    remote_hps = qemu_get_be64(mis->from_src_file);

    if (remote_hps != getpagesize())  {

        

        error_report("Postcopy needs matching host page sizes (s=%d d=%d)",

                     (int)remote_hps, getpagesize());

        return -1;

    }



    remote_tps = qemu_get_be64(mis->from_src_file);

    if (remote_tps != (1ul << qemu_target_page_bits())) {

        

        error_report("Postcopy needs matching target page sizes (s=%d d=%d)",

                     (int)remote_tps, 1 << qemu_target_page_bits());

        return -1;

    }



    if (ram_postcopy_incoming_init(mis)) {

        return -1;

    }



    postcopy_state_set(POSTCOPY_INCOMING_ADVISE);



    return 0;

}