static void qemu_kill_report(void)

{

    if (!qtest_driver() && shutdown_signal != -1) {

        fprintf(stderr, "qemu: terminating on signal %d", shutdown_signal);

        if (shutdown_pid == 0) {

            

            fputc('\n', stderr);

        } else {

            fprintf(stderr, " from pid " FMT_pid "\n", shutdown_pid);

        }

        shutdown_signal = -1;

    }

}