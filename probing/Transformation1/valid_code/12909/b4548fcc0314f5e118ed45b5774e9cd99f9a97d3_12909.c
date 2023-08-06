grlib_apbuart_writel(void *opaque, target_phys_addr_t addr, uint32_t value)

{

    UART          *uart = opaque;

    unsigned char  c    = 0;



    addr &= 0xff;



    

    switch (addr) {

    case DATA_OFFSET:

        c = value & 0xFF;

        qemu_chr_write(uart->chr, &c, 1);

        return;



    case STATUS_OFFSET:

        

        return;



    case CONTROL_OFFSET:

        

        return;



    case SCALER_OFFSET:

        

        return;



    default:

        break;

    }



    trace_grlib_apbuart_unknown_register("write", addr);

}