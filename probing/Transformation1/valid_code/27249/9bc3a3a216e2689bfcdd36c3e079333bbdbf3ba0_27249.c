static int ehci_state_waitlisthead(EHCIState *ehci,  int async)

{

    EHCIqh qh;

    int i = 0;

    int again = 0;

    uint32_t entry = ehci->asynclistaddr;



    

    if (async) {

        ehci_set_usbsts(ehci, USBSTS_REC);

    }



    ehci_queues_rip_unused(ehci, async, 0);



    

    for(i = 0; i < MAX_QH; i++) {

        get_dwords(ehci, NLPTR_GET(entry), (uint32_t *) &qh,

                   sizeof(EHCIqh) >> 2);

        ehci_trace_qh(NULL, NLPTR_GET(entry), &qh);



        if (qh.epchar & QH_EPCHAR_H) {

            if (async) {

                entry |= (NLPTR_TYPE_QH << 1);

            }



            ehci_set_fetch_addr(ehci, async, entry);

            ehci_set_state(ehci, async, EST_FETCHENTRY);

            again = 1;

            goto out;

        }



        entry = qh.next;

        if (entry == ehci->asynclistaddr) {

            break;

        }

    }



    



    ehci_set_state(ehci, async, EST_ACTIVE);



out:

    return again;

}