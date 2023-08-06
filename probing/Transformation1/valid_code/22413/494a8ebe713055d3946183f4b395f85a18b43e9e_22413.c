static int v9fs_receivefd(int sockfd, int *status)

{

    struct iovec iov;

    struct msghdr msg;

    struct cmsghdr *cmsg;

    int retval, data, fd;

    union MsgControl msg_control;



    iov.iov_base = &data;

    iov.iov_len = sizeof(data);



    memset(&msg, 0, sizeof(msg));

    msg.msg_iov = &iov;

    msg.msg_iovlen = 1;

    msg.msg_control = &msg_control;

    msg.msg_controllen = sizeof(msg_control);



    do {

        retval = recvmsg(sockfd, &msg, 0);

    } while (retval < 0 && errno == EINTR);

    if (retval <= 0) {

        return retval;

    }

    

    if (data != V9FS_FD_VALID) {

        *status = data;

        return 0;

    }

    

    for (cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {

        if (cmsg->cmsg_len != CMSG_LEN(sizeof(int)) ||

            cmsg->cmsg_level != SOL_SOCKET ||

            cmsg->cmsg_type != SCM_RIGHTS) {

            continue;

        }

        fd = *((int *)CMSG_DATA(cmsg));

        *status = fd;

        return 0;

    }

    *status = -ENFILE;  

    return 0;

}