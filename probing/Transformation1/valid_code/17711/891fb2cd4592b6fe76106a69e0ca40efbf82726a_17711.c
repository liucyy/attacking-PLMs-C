static void handle_port_status_write(EHCIState *s, int port, uint32_t val)

{

    uint32_t *portsc = &s->portsc[port];

    USBDevice *dev = s->ports[port].dev;



    

    *portsc &= ~(val & PORTSC_RWC_MASK);

    

    *portsc &= val | ~PORTSC_PED;

    

    handle_port_owner_write(s, port, val);

    

    val &= PORTSC_RO_MASK;



    if ((val & PORTSC_PRESET) && !(*portsc & PORTSC_PRESET)) {

        trace_usb_ehci_port_reset(port, 1);

    }



    if (!(val & PORTSC_PRESET) &&(*portsc & PORTSC_PRESET)) {

        trace_usb_ehci_port_reset(port, 0);

        if (dev) {

            usb_attach(&s->ports[port], dev);

            usb_send_msg(dev, USB_MSG_RESET);

            *portsc &= ~PORTSC_CSC;

        }



        

        if (dev && (dev->speedmask & USB_SPEED_MASK_HIGH)) {

            val |= PORTSC_PED;

        }

    }



    *portsc &= ~PORTSC_RO_MASK;

    *portsc |= val;

}