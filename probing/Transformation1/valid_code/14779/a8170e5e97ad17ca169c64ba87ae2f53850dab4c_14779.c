static uint64_t grlib_apbuart_read(void *opaque, target_phys_addr_t addr,

                                   unsigned size)

{

    UART     *uart = opaque;



    addr &= 0xff;



    

    switch (addr) {

    case DATA_OFFSET:

    case DATA_OFFSET + 3:       

        return uart_pop(uart);



    case STATUS_OFFSET:

        

        return uart->status;



    case CONTROL_OFFSET:

        return uart->control;



    case SCALER_OFFSET:

        

        return 0;



    default:

        trace_grlib_apbuart_readl_unknown(addr);

        return 0;

    }

}