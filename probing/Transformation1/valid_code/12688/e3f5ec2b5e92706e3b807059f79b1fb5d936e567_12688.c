static void smc91c111_receive(void *opaque, const uint8_t *buf, size_t size)

{

    smc91c111_state *s = (smc91c111_state *)opaque;

    int status;

    int packetsize;

    uint32_t crc;

    int packetnum;

    uint8_t *p;



    if ((s->rcr & RCR_RXEN) == 0 || (s->rcr & RCR_SOFT_RST))

        return;

    

    if (size < 64)

        packetsize = 64;

    else

        packetsize = (size & ~1);

    packetsize += 6;

    crc = (s->rcr & RCR_STRIP_CRC) == 0;

    if (crc)

        packetsize += 4;

    

    if (packetsize > 2048)

        return;

    packetnum = smc91c111_allocate_packet(s);

    if (packetnum == 0x80)

        return;

    s->rx_fifo[s->rx_fifo_len++] = packetnum;



    p = &s->data[packetnum][0];

    

    status = 0;

    if (size > 1518)

        status |= RS_TOOLONG;

    if (size & 1)

        status |= RS_ODDFRAME;

    *(p++) = status & 0xff;

    *(p++) = status >> 8;

    *(p++) = packetsize & 0xff;

    *(p++) = packetsize >> 8;

    memcpy(p, buf, size & ~1);

    p += (size & ~1);

    

    if (size < 64) {

        int pad;



        if (size & 1)

            *(p++) = buf[size - 1];

        pad = 64 - size;

        memset(p, 0, pad);

        p += pad;

        size = 64;

    }

    

    if (crc) {

        crc = crc32(~0, buf, size);

        *(p++) = crc & 0xff; crc >>= 8;

        *(p++) = crc & 0xff; crc >>= 8;

        *(p++) = crc & 0xff; crc >>= 8;

        *(p++) = crc & 0xff; crc >>= 8;

    }

    if (size & 1) {

        *(p++) = buf[size - 1];

        *(p++) = 0x60;

    } else {

        *(p++) = 0;

        *(p++) = 0x40;

    }

    

    s->int_level |= INT_RCV;

    smc91c111_update(s);

}