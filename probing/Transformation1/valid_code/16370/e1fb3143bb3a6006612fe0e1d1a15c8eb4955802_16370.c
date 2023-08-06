static int ftp_abort(URLContext *h)

{

    static const char *command = "ABOR\r\n";

    int err;

    static const int abor_codes[] = {225, 226, 0};

    FTPContext *s = h->priv_data;



    



    if (ftp_send_command(s, command, NULL, NULL) < 0) {

        ftp_close_both_connections(s);

        if ((err = ftp_connect_control_connection(h)) < 0) {

            av_log(h, AV_LOG_ERROR, "Reconnect failed.\n");

            return err;

        }

    } else {

        ftp_close_data_connection(s);

    }



    if (ftp_status(s, NULL, abor_codes) < 225) {

        

        ffurl_closep(&s->conn_control);

        if ((err = ftp_connect_control_connection(h)) < 0) {

            av_log(h, AV_LOG_ERROR, "Reconnect failed.\n");

            return err;

        }

    }



    return 0;

}