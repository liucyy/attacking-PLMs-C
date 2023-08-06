static int send_status(int sockfd, struct iovec *iovec, int status)

{

    ProxyHeader header;

    int retval, msg_size;



    if (status < 0) {

        header.type = T_ERROR;

    } else {

        header.type = T_SUCCESS;


    header.size = sizeof(status);

    

    msg_size = proxy_marshal(iovec, 0, "ddd", header.type,

                             header.size, status);




    retval = socket_write(sockfd, iovec->iov_base, msg_size);

    if (retval < 0) {

        return retval;


    return 0;