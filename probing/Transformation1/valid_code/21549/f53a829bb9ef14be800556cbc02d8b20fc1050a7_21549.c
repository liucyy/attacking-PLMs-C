void nbd_client_close(NBDClient *client)

{

    if (client->closing) {

        return;

    }



    client->closing = true;



    

    shutdown(client->sock, 2);



    

    if (client->close) {

        client->close(client);

    }

}