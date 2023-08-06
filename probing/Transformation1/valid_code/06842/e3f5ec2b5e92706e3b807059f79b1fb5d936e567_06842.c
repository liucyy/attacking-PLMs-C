static void stellaris_enet_receive(void *opaque, const uint8_t *buf, size_t size)

{

    stellaris_enet_state *s = (stellaris_enet_state *)opaque;

    int n;

    uint8_t *p;

    uint32_t crc;



    if ((s->rctl & SE_RCTL_RXEN) == 0)

        return;

    if (s->np >= 31) {

        DPRINTF("Packet dropped\n");

        return;

    }



    DPRINTF("Received packet len=%d\n", size);

    n = s->next_packet + s->np;

    if (n >= 31)

        n -= 31;

    s->np++;



    s->rx[n].len = size + 6;

    p = s->rx[n].data;

    *(p++) = (size + 6);

    *(p++) = (size + 6) >> 8;

    memcpy (p, buf, size);

    p += size;

    crc = crc32(~0, buf, size);

    *(p++) = crc;

    *(p++) = crc >> 8;

    *(p++) = crc >> 16;

    *(p++) = crc >> 24;

    

    if ((size & 3) != 2) {

        memset(p, 0, (6 - size) & 3);

    }



    s->ris |= SE_INT_RX;

    stellaris_enet_update(s);

}