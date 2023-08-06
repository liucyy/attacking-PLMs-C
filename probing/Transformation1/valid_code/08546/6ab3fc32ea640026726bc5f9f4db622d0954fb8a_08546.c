static void digic_uart_write(void *opaque, hwaddr addr, uint64_t value,

                             unsigned size)

{

    DigicUartState *s = opaque;

    unsigned char ch = value;



    addr >>= 2;



    switch (addr) {

    case R_TX:

        if (s->chr) {



            qemu_chr_fe_write_all(s->chr, &ch, 1);

        }

        break;



    case R_ST:

        

        break;



    default:

        qemu_log_mask(LOG_UNIMP,

                      "digic-uart: write access to unknown register 0x"

                      TARGET_FMT_plx, addr << 2);

    }

}