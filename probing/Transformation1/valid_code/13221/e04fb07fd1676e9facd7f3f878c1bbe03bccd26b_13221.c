static int qemu_rbd_send_pipe(BDRVRBDState *s, RADOSCB *rcb)

{

    int ret = 0;

    while (1) {

        fd_set wfd;

        int fd = s->fds[RBD_FD_WRITE];



        

        ret = write(fd, (void *)&rcb, sizeof(rcb));

        if (ret >= 0) {

            break;

        }

        if (errno == EINTR) {

            continue;

        }

        if (errno != EAGAIN) {

            break;

        }



        FD_ZERO(&wfd);

        FD_SET(fd, &wfd);

        do {

            ret = select(fd + 1, NULL, &wfd, NULL, NULL);

        } while (ret < 0 && errno == EINTR);

    }



    return ret;

}