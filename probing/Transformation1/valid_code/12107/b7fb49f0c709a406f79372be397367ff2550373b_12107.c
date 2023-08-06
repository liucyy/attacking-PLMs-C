int dpy_set_ui_info(QemuConsole *con, QemuUIInfo *info)

{

    assert(con != NULL);

    con->ui_info = *info;

    if (!con->hw_ops->ui_info) {

        return -1;

    }



    

    timer_mod(con->ui_timer, qemu_clock_get_ms(QEMU_CLOCK_REALTIME) + 1000);

    return 0;

}