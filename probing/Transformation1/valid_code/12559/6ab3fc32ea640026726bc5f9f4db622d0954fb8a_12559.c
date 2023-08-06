static void grlib_apbuart_write(void *opaque, hwaddr addr,

                                uint64_t value, unsigned size)

{

    UART          *uart = opaque;

    unsigned char  c    = 0;



    addr &= 0xff;



    

    switch (addr) {

    case DATA_OFFSET:

    case DATA_OFFSET + 3:       

        

        if ((uart->chr) && (uart->control & UART_TRANSMIT_ENABLE)) {

            c = value & 0xFF;

            qemu_chr_fe_write(uart->chr, &c, 1);

            

            if (uart->control & UART_TRANSMIT_INTERRUPT) {

                qemu_irq_pulse(uart->irq);

            }

        }

        return;



    case STATUS_OFFSET:

        

        return;



    case CONTROL_OFFSET:

        uart->control = value;

        return;



    case SCALER_OFFSET:

        

        return;



    default:

        break;

    }



    trace_grlib_apbuart_writel_unknown(addr, value);

}