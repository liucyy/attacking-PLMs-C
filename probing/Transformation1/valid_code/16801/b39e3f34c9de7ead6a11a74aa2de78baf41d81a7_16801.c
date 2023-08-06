static void qemu_account_warp_timer(void)

{

    if (!use_icount || !icount_sleep) {

        return;

    }



    

    if (!runstate_is_running()) {

        return;

    }



    

    if (!replay_checkpoint(CHECKPOINT_CLOCK_WARP_ACCOUNT)) {

        return;

    }



    timer_del(icount_warp_timer);

    icount_warp_rt();

}