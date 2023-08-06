static int uhci_complete_td(UHCIState *s, UHCI_TD *td, UHCIAsync *async, uint32_t *int_mask)

{

    int len = 0, max_len, err, ret;

    uint8_t pid;



    max_len = ((td->token >> 21) + 1) & 0x7ff;

    pid = td->token & 0xff;



    ret = async->packet.len;



    if (td->ctrl & TD_CTRL_IOS)

        td->ctrl &= ~TD_CTRL_ACTIVE;



    if (ret < 0)

        goto out;



    len = async->packet.len;

    td->ctrl = (td->ctrl & ~0x7ff) | ((len - 1) & 0x7ff);



    

    td->ctrl &= ~(TD_CTRL_ACTIVE | TD_CTRL_NAK);

    if (td->ctrl & TD_CTRL_IOC)

        *int_mask |= 0x01;



    if (pid == USB_TOKEN_IN) {

        if (len > max_len) {

            ret = USB_RET_BABBLE;

            goto out;

        }



        if (len > 0) {

            

            cpu_physical_memory_write(td->buffer, async->buffer, len);

        }



        if ((td->ctrl & TD_CTRL_SPD) && len < max_len) {

            *int_mask |= 0x02;

            

            DPRINTF("uhci: short packet. td 0x%x token 0x%x\n", async->td, async->token);

            return 1;

        }

    }



    

    return 0;



out:

    switch(ret) {

    case USB_RET_STALL:

        td->ctrl |= TD_CTRL_STALL;

        td->ctrl &= ~TD_CTRL_ACTIVE;

        s->status |= UHCI_STS_USBERR;

        if (td->ctrl & TD_CTRL_IOC) {

            *int_mask |= 0x01;

        }

        uhci_update_irq(s);

        return 1;



    case USB_RET_BABBLE:

        td->ctrl |= TD_CTRL_BABBLE | TD_CTRL_STALL;

        td->ctrl &= ~TD_CTRL_ACTIVE;

        s->status |= UHCI_STS_USBERR;

        if (td->ctrl & TD_CTRL_IOC) {

            *int_mask |= 0x01;

        }

        uhci_update_irq(s);

        

        return -1;



    case USB_RET_NAK:

        td->ctrl |= TD_CTRL_NAK;

        if (pid == USB_TOKEN_SETUP)

            break;

	return 1;



    case USB_RET_NODEV:

    default:

	break;

    }



    



    td->ctrl |= TD_CTRL_TIMEOUT;

    err = (td->ctrl >> TD_CTRL_ERROR_SHIFT) & 3;

    if (err != 0) {

        err--;

        if (err == 0) {

            td->ctrl &= ~TD_CTRL_ACTIVE;

            s->status |= UHCI_STS_USBERR;

            if (td->ctrl & TD_CTRL_IOC)

                *int_mask |= 0x01;

            uhci_update_irq(s);

        }

    }

    td->ctrl = (td->ctrl & ~(3 << TD_CTRL_ERROR_SHIFT)) |

        (err << TD_CTRL_ERROR_SHIFT);

    return 1;

}