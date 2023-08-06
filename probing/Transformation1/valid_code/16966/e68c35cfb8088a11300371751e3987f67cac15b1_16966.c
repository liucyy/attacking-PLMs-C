static int nbd_negotiate_handle_list(NBDClient *client, uint32_t length,

                                     Error **errp)

{

    NBDExport *exp;



    if (length) {

        if (nbd_drop(client->ioc, length, errp) < 0) {

            return -EIO;

        }

        return nbd_negotiate_send_rep_err(client->ioc,

                                          NBD_REP_ERR_INVALID, NBD_OPT_LIST,

                                          errp,

                                          "OPT_LIST should not have length");

    }



    

    QTAILQ_FOREACH(exp, &exports, next) {

        if (nbd_negotiate_send_rep_list(client->ioc, exp, errp)) {

            return -EINVAL;

        }

    }

    

    return nbd_negotiate_send_rep(client->ioc, NBD_REP_ACK, NBD_OPT_LIST, errp);

}