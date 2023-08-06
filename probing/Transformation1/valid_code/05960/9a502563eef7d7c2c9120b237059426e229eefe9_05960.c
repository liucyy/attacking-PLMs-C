static void cmd_inquiry(IDEState *s, uint8_t *buf)

{

    int max_len = buf[4];



    buf[0] = 0x05; 

    buf[1] = 0x80; 

    buf[2] = 0x00; 

    buf[3] = 0x21; 

    buf[4] = 31; 

    buf[5] = 0; 

    buf[6] = 0; 

    buf[7] = 0; 

    padstr8(buf + 8, 8, "QEMU");

    padstr8(buf + 16, 16, "QEMU DVD-ROM");

    padstr8(buf + 32, 4, s->version);

    ide_atapi_cmd_reply(s, 36, max_len);

}