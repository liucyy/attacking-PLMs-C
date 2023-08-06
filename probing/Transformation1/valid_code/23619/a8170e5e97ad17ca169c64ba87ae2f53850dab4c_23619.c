static void uart_write(void *opaque, target_phys_addr_t offset,

                          uint64_t value, unsigned size)

{

    UartState *s = (UartState *)opaque;



    DB_PRINT(" offset:%x data:%08x\n", offset, (unsigned)value);

    offset >>= 2;

    switch (offset) {

    case R_IER: 

        s->r[R_IMR] |= value;

        break;

    case R_IDR: 

        s->r[R_IMR] &= ~value;

        break;

    case R_IMR: 

        break;

    case R_CISR: 

        s->r[R_CISR] &= ~value;

        break;

    case R_TX_RX: 

        switch (s->r[R_MR] & UART_MR_CHMODE) {

        case NORMAL_MODE:

            uart_write_tx_fifo(s, (uint8_t *) &value, 1);

            break;

        case LOCAL_LOOPBACK:

            uart_write_rx_fifo(opaque, (uint8_t *) &value, 1);

            break;

        }

        break;

    default:

        s->r[offset] = value;

    }



    switch (offset) {

    case R_CR:

        uart_ctrl_update(s);

        break;

    case R_MR:

        uart_parameters_setup(s);

        break;

    }

}