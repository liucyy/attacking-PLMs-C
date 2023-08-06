static void ehci_opreg_write(void *ptr, hwaddr addr,

                             uint64_t val, unsigned size)

{

    EHCIState *s = ptr;

    uint32_t *mmio = s->opreg + (addr >> 2);

    uint32_t old = *mmio;

    int i;



    trace_usb_ehci_opreg_write(addr + s->opregbase, addr2str(addr), val);



    switch (addr) {

    case USBCMD:

        if (val & USBCMD_HCRESET) {

            ehci_reset(s);

            val = s->usbcmd;

            break;

        }



        

        if ((val & USBCMD_FLS) && !(s->usbcmd & USBCMD_FLS)) {

            fprintf(stderr, "attempt to set frame list size -- value %d\n",

                    (int)val & USBCMD_FLS);

            val &= ~USBCMD_FLS;

        }



        if (val & USBCMD_IAAD) {

            

            s->async_stepdown = 0;

            qemu_bh_schedule(s->async_bh);

            trace_usb_ehci_doorbell_ring();

        }



        if (((USBCMD_RUNSTOP | USBCMD_PSE | USBCMD_ASE) & val) !=

            ((USBCMD_RUNSTOP | USBCMD_PSE | USBCMD_ASE) & s->usbcmd)) {

            if (s->pstate == EST_INACTIVE) {

                SET_LAST_RUN_CLOCK(s);

            }

            s->usbcmd = val; 

            ehci_update_halt(s);

            s->async_stepdown = 0;

            qemu_bh_schedule(s->async_bh);

        }

        break;



    case USBSTS:

        val &= USBSTS_RO_MASK;              

        ehci_clear_usbsts(s, val);          

        val = s->usbsts;

        ehci_update_irq(s);

        break;



    case USBINTR:

        val &= USBINTR_MASK;

        if (ehci_enabled(s) && (USBSTS_FLR & val)) {

            qemu_bh_schedule(s->async_bh);

        }

        break;



    case FRINDEX:

        val &= 0x00003ff8; 

        break;



    case CONFIGFLAG:

        val &= 0x1;

        if (val) {

            for(i = 0; i < NB_PORTS; i++)

                handle_port_owner_write(s, i, 0);

        }

        break;



    case PERIODICLISTBASE:

        if (ehci_periodic_enabled(s)) {

            fprintf(stderr,

              "ehci: PERIODIC list base register set while periodic schedule\n"

              "      is enabled and HC is enabled\n");

        }

        break;



    case ASYNCLISTADDR:

        if (ehci_async_enabled(s)) {

            fprintf(stderr,

              "ehci: ASYNC list address register set while async schedule\n"

              "      is enabled and HC is enabled\n");

        }

        break;

    }



    *mmio = val;

    trace_usb_ehci_opreg_change(addr + s->opregbase, addr2str(addr),

                                *mmio, old);

}