static int ftp_send_command(FTPContext *s, const char *command,

                            const int response_codes[], char **response)

{

    int err;



    

    if ((err = ftp_flush_control_input(s)) < 0)

        return err;



    

    s->conn_control_block_flag = 0;

    if ((err = ffurl_write(s->conn_control, command, strlen(command))) < 0)

        return err;

    if (!err)

        return -1;



    

    if (response_codes) {

        return ftp_status(s, response, response_codes);

    }

    return 0;

}