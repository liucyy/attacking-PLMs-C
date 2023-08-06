static int ehci_state_executing(EHCIQueue *q)

{

    EHCIPacket *p = QTAILQ_FIRST(&q->packets);



    assert(p != NULL);

    assert(p->qtdaddr == q->qtdaddr);



    ehci_execute_complete(q);



    

    if (!q->async) {

        int transactCtr = get_field(q->qh.epcap, QH_EPCAP_MULT);

        transactCtr--;

        set_field(&q->qh.epcap, transactCtr, QH_EPCAP_MULT);

        

        

    }



    

    if (p->usb_status == USB_RET_NAK) {

        ehci_set_state(q->ehci, q->async, EST_HORIZONTALQH);

    } else {

        ehci_set_state(q->ehci, q->async, EST_WRITEBACK);

    }



    ehci_flush_qh(q);

    return 1;

}