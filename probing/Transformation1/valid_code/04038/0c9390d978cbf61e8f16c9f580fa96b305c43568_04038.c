static void client_close(NBDClient *client)

{

    if (client->closing) {

        return;

    }



    client->closing = true;



    

    qio_channel_shutdown(client->ioc, QIO_CHANNEL_SHUTDOWN_BOTH,

                         NULL);



    

    if (client->close) {

        client->close(client);

    }

}