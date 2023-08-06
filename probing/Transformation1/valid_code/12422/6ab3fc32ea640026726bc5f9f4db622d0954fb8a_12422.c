static ssize_t write_console_data(SCLPEvent *event, const uint8_t *buf,

                                  size_t len)

{

    SCLPConsole *scon = SCLP_CONSOLE(event);



    if (!scon->chr) {

        

        return len;

    }





    return qemu_chr_fe_write_all(scon->chr, buf, len);

}