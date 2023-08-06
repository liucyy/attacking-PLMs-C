int postcopy_ram_enable_notify(MigrationIncomingState *mis)

{

    

    mis->userfault_fd = syscall(__NR_userfaultfd, O_CLOEXEC | O_NONBLOCK);

    if (mis->userfault_fd == -1) {

        error_report("%s: Failed to open userfault fd: %s", __func__,

                     strerror(errno));

        return -1;

    }



    

    if (!ufd_version_check(mis->userfault_fd)) {

        return -1;

    }



    

    mis->userfault_quit_fd = eventfd(0, EFD_CLOEXEC);

    if (mis->userfault_quit_fd == -1) {

        error_report("%s: Opening userfault_quit_fd: %s", __func__,

                     strerror(errno));

        close(mis->userfault_fd);

        return -1;

    }



    qemu_sem_init(&mis->fault_thread_sem, 0);

    qemu_thread_create(&mis->fault_thread, "postcopy/fault",

                       postcopy_ram_fault_thread, mis, QEMU_THREAD_JOINABLE);

    qemu_sem_wait(&mis->fault_thread_sem);

    qemu_sem_destroy(&mis->fault_thread_sem);

    mis->have_fault_thread = true;



    

    if (qemu_ram_foreach_block(ram_block_enable_notify, mis)) {

        return -1;

    }



    

    qemu_balloon_inhibit(true);



    trace_postcopy_ram_enable_notify();



    return 0;

}