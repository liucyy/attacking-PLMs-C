static void mcf_fec_receive(void *opaque, const uint8_t *buf, size_t size)

{

    mcf_fec_state *s = (mcf_fec_state *)opaque;

    mcf_fec_bd bd;

    uint32_t flags = 0;

    uint32_t addr;

    uint32_t crc;

    uint32_t buf_addr;

    uint8_t *crc_ptr;

    unsigned int buf_len;



    DPRINTF("do_rx len %d\n", size);

    if (!s->rx_enabled) {

        fprintf(stderr, "mcf_fec_receive: Unexpected packet\n");

    }

    

    size += 4;

    crc = cpu_to_be32(crc32(~0, buf, size));

    crc_ptr = (uint8_t *)&crc;

    

    if (size > FEC_MAX_FRAME_SIZE) {

        size = FEC_MAX_FRAME_SIZE;

        flags |= FEC_BD_TR | FEC_BD_LG;

    }

    

    if (size > (s->rcr >> 16)) {

        flags |= FEC_BD_LG;

    }

    addr = s->rx_descriptor;

    while (size > 0) {

        mcf_fec_read_bd(&bd, addr);

        if ((bd.flags & FEC_BD_E) == 0) {

            

            

            fprintf(stderr, "mcf_fec: Lost end of frame\n");

            break;

        }

        buf_len = (size <= s->emrbr) ? size: s->emrbr;

        bd.length = buf_len;

        size -= buf_len;

        DPRINTF("rx_bd %x length %d\n", addr, bd.length);

        

        if (size < 4)

            buf_len += size - 4;

        buf_addr = bd.data;

        cpu_physical_memory_write(buf_addr, buf, buf_len);

        buf += buf_len;

        if (size < 4) {

            cpu_physical_memory_write(buf_addr + buf_len, crc_ptr, 4 - size);

            crc_ptr += 4 - size;

        }

        bd.flags &= ~FEC_BD_E;

        if (size == 0) {

            

            bd.flags |= flags | FEC_BD_L;

            DPRINTF("rx frame flags %04x\n", bd.flags);

            s->eir |= FEC_INT_RXF;

        } else {

            s->eir |= FEC_INT_RXB;

        }

        mcf_fec_write_bd(&bd, addr);

        

        if ((bd.flags & FEC_BD_W) != 0) {

            addr = s->erdsr;

        } else {

            addr += 8;

        }

    }

    s->rx_descriptor = addr;

    mcf_fec_enable_rx(s);

    mcf_fec_update(s);

}