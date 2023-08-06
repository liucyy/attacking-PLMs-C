static void xhci_port_write(void *ptr, hwaddr reg,

                            uint64_t val, unsigned size)

{

    XHCIPort *port = ptr;

    uint32_t portsc;



    trace_usb_xhci_port_write(port->portnr, reg, val);



    switch (reg) {

    case 0x00: 

        portsc = port->portsc;

        

        portsc &= ~(val & (PORTSC_CSC|PORTSC_PEC|PORTSC_WRC|PORTSC_OCC|

                           PORTSC_PRC|PORTSC_PLC|PORTSC_CEC));

        if (val & PORTSC_LWS) {

            

            uint32_t pls = get_field(val, PORTSC_PLS);

            set_field(&portsc, pls, PORTSC_PLS);

            trace_usb_xhci_port_link(port->portnr, pls);

        }

        

        portsc &= ~(PORTSC_PP|PORTSC_WCE|PORTSC_WDE|PORTSC_WOE);

        portsc |= (val & (PORTSC_PP|PORTSC_WCE|PORTSC_WDE|PORTSC_WOE));

        port->portsc = portsc;

        

        if (val & PORTSC_PR) {

            xhci_port_reset(port);

        }

        break;

    case 0x04: 

    case 0x08: 

    default:

        trace_usb_xhci_unimplemented("port write", reg);

    }

}