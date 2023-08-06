static void tcp_chr_telnet_init(QIOChannel *ioc)

{

    char buf[3];

    

    IACSET(buf, 0xff, 0xfb, 0x01);  

    qio_channel_write(ioc, buf, 3, NULL);

    IACSET(buf, 0xff, 0xfb, 0x03);  

    qio_channel_write(ioc, buf, 3, NULL);

    IACSET(buf, 0xff, 0xfb, 0x00);  

    qio_channel_write(ioc, buf, 3, NULL);

    IACSET(buf, 0xff, 0xfd, 0x00);  

    qio_channel_write(ioc, buf, 3, NULL);

}