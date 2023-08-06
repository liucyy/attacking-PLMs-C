static int nbd_open(BlockDriverState *bs, QDict *options, int flags,

                    Error **errp)

{

    BDRVNBDState *s = bs->opaque;

    char *export = NULL;

    int result, sock;

    Error *local_err = NULL;



    

    nbd_config(s, options, &export, &local_err);

    if (local_err) {

        error_propagate(errp, local_err);

        return -EINVAL;

    }



    

    sock = nbd_establish_connection(bs, errp);

    if (sock < 0) {


        return sock;

    }



    

    result = nbd_client_init(bs, sock, export, errp);


    return result;

}