static int ehci_state_writeback(EHCIQueue *q, int async)

{

    int again = 0;



    

    ehci_trace_qtd(q, NLPTR_GET(q->qtdaddr), (EHCIqtd*) &q->qh.next_qtd);

    put_dwords(NLPTR_GET(q->qtdaddr),(uint32_t *) &q->qh.next_qtd,

                sizeof(EHCIqtd) >> 2);



    

    if (q->qh.token & QTD_TOKEN_HALT) {

        ehci_set_state(q->ehci, async, EST_HORIZONTALQH);

        again = 1;

    } else {

        ehci_set_state(q->ehci, async, EST_ADVANCEQUEUE);

        again = 1;

    }

    return again;

}