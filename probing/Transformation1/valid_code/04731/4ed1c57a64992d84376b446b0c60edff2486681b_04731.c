static int ehci_state_writeback(EHCIQueue *q)

{

    EHCIPacket *p = QTAILQ_FIRST(&q->packets);

    int again = 0;



    

    assert(p != NULL);

    assert(p->qtdaddr == q->qtdaddr);



    ehci_trace_qtd(q, NLPTR_GET(p->qtdaddr), (EHCIqtd *) &q->qh.next_qtd);

    put_dwords(q->ehci, NLPTR_GET(p->qtdaddr), (uint32_t *) &q->qh.next_qtd,

               sizeof(EHCIqtd) >> 2);

    ehci_free_packet(p);



    

    if (q->qh.token & QTD_TOKEN_HALT) {

        ehci_set_state(q->ehci, q->async, EST_HORIZONTALQH);

        again = 1;

    } else {

        ehci_set_state(q->ehci, q->async, EST_ADVANCEQUEUE);

        again = 1;

    }

    return again;

}