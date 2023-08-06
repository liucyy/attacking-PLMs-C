void qemu_system_killed(int signal, pid_t pid)

{

    shutdown_signal = signal;

    shutdown_pid = pid;

    no_shutdown = 0;



    

    shutdown_requested = 1;

    qemu_notify_event();

}