static int uhci_handle_td(UHCIState *s, UHCIQueue *q, uint32_t qh_addr,

                          UHCI_TD *td, uint32_t td_addr, uint32_t *int_mask)

{

    int ret, max_len;

    bool spd;

    bool queuing = (q != NULL);

    uint8_t pid = td->token & 0xff;

    UHCIAsync *async;



    switch (pid) {

    case USB_TOKEN_OUT:

    case USB_TOKEN_SETUP:

    case USB_TOKEN_IN:

        break;

    default:

        

        s->status |= UHCI_STS_HCPERR;

        s->cmd &= ~UHCI_CMD_RS;

        uhci_update_irq(s);

        return TD_RESULT_STOP_FRAME;

    }



    async = uhci_async_find_td(s, td_addr);

    if (async) {

        if (uhci_queue_verify(async->queue, qh_addr, td, td_addr, queuing)) {

            assert(q == NULL || q == async->queue);

            q = async->queue;

        } else {

            uhci_queue_free(async->queue, "guest re-used pending td");

            async = NULL;

        }

    }



    if (q == NULL) {

        q = uhci_queue_find(s, td);

        if (q && !uhci_queue_verify(q, qh_addr, td, td_addr, queuing)) {

            uhci_queue_free(q, "guest re-used qh");

            q = NULL;

        }

    }



    if (q) {

        q->valid = QH_VALID;

    }



    

    if (!(td->ctrl & TD_CTRL_ACTIVE)) {

        if (async) {

            

            uhci_queue_free(async->queue, "pending td non-active");

        }

        

        if (td->ctrl & TD_CTRL_IOC) {

                *int_mask |= 0x01;

        }

        return TD_RESULT_NEXT_QH;

    }



    if (async) {

        if (queuing) {

            

            return TD_RESULT_ASYNC_CONT;

        }

        if (!async->done) {

            UHCI_TD last_td;

            UHCIAsync *last = QTAILQ_LAST(&async->queue->asyncs, asyncs_head);

            

            uhci_read_td(s, &last_td, last->td_addr);

            uhci_queue_fill(async->queue, &last_td);



            return TD_RESULT_ASYNC_CONT;

        }

        uhci_async_unlink(async);

        goto done;

    }



    if (s->completions_only) {

        return TD_RESULT_ASYNC_CONT;

    }



    

    if (q == NULL) {

        USBDevice *dev = uhci_find_device(s, (td->token >> 8) & 0x7f);

        USBEndpoint *ep = usb_ep_get(dev, pid, (td->token >> 15) & 0xf);



        if (ep == NULL) {

            return uhci_handle_td_error(s, td, td_addr, USB_RET_NODEV,

                                        int_mask);

        }

        q = uhci_queue_new(s, qh_addr, td, ep);

    }

    async = uhci_async_alloc(q, td_addr);



    max_len = ((td->token >> 21) + 1) & 0x7ff;

    spd = (pid == USB_TOKEN_IN && (td->ctrl & TD_CTRL_SPD) != 0);

    usb_packet_setup(&async->packet, pid, q->ep, 0, td_addr, spd,

                     (td->ctrl & TD_CTRL_IOC) != 0);

    if (max_len <= sizeof(async->static_buf)) {

        async->buf = async->static_buf;

    } else {

        async->buf = g_malloc(max_len);

    }

    usb_packet_addbuf(&async->packet, async->buf, max_len);



    switch(pid) {

    case USB_TOKEN_OUT:

    case USB_TOKEN_SETUP:

        pci_dma_read(&s->dev, td->buffer, async->buf, max_len);

        usb_handle_packet(q->ep->dev, &async->packet);

        if (async->packet.status == USB_RET_SUCCESS) {

            async->packet.actual_length = max_len;

        }

        break;



    case USB_TOKEN_IN:

        usb_handle_packet(q->ep->dev, &async->packet);

        break;



    default:

        abort(); 

    }



    if (async->packet.status == USB_RET_ASYNC) {

        uhci_async_link(async);

        if (!queuing) {

            uhci_queue_fill(q, td);

        }

        return TD_RESULT_ASYNC_START;

    }



done:

    ret = uhci_complete_td(s, td, async, int_mask);

    uhci_async_free(async);

    return ret;

}