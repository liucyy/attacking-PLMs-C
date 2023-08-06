static void xen_domain_watcher(void)

{

    int qemu_running = 1;

    int fd[2], i, n, rc;

    char byte;



    pipe(fd);

    if (fork() != 0)

        return; 



    

    n = getdtablesize();

    for (i = 3; i < n; i++) {

        if (i == fd[0])

            continue;

        if (i == xen_xc)

            continue;

        close(i);

    }



    

    signal(SIGINT,  SIG_IGN);

    signal(SIGTERM, SIG_IGN);



    

    while (qemu_running) {

        rc = read(fd[0], &byte, 1);

        switch (rc) {

        case -1:

            if (errno == EINTR)

                continue;

            qemu_log("%s: Huh? read error: %s\n", __FUNCTION__, strerror(errno));

            qemu_running = 0;

            break;

        case 0:

            

            qemu_running = 0;

            break;

        default:

            qemu_log("%s: Huh? data on the watch pipe?\n", __FUNCTION__);

            break;

        }

    }



    

    qemu_log("%s: destroy domain %d\n", __FUNCTION__, xen_domid);

    xc_domain_destroy(xen_xc, xen_domid);

    _exit(0);

}