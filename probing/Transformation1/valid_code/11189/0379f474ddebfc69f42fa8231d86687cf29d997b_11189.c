static int nbd_handle_list(NBDClient *client, uint32_t length)

{

    int csock;

    NBDExport *exp;



    csock = client->sock;

    if (length) {




        return nbd_send_rep(csock, NBD_REP_ERR_INVALID, NBD_OPT_LIST);




    

    QTAILQ_FOREACH(exp, &exports, next) {

        if (nbd_send_rep_list(csock, exp)) {

            return -EINVAL;



    

    return nbd_send_rep(csock, NBD_REP_ACK, NBD_OPT_LIST);