static int write_console_data(SCLPEvent *event, const uint8_t *buf, int len)

{

    int ret = 0;

    const uint8_t *buf_offset;



    SCLPConsoleLM *scon = SCLPLM_CONSOLE(event);



    if (!scon->chr) {

        

        return len;

    }



    buf_offset = buf;

    while (len > 0) {

        ret = qemu_chr_fe_write(scon->chr, buf, len);

        if (ret == 0) {

            

            len = 0;

        } else if (ret == -EAGAIN || (ret > 0 && ret < len)) {

            len -= ret;

            buf_offset += ret;

        } else {

            len = 0;

        }

    }



    return ret;

}