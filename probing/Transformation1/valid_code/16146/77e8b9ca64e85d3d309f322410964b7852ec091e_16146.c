static int nbd_open(BlockDriverState *bs, QDict *options, int flags,

                    Error **errp)

{

    BDRVNBDState *s = bs->opaque;

    char *export = NULL;

    int result, sock;



    

    result = nbd_config(s, options, &export);

    if (result != 0) {

        return result;

    }



    

    sock = nbd_establish_connection(bs);

    if (sock < 0) {

        return sock;

    }



    

    result = nbd_client_session_init(&s->client, bs, sock, export);

    g_free(export);

    return result;

}