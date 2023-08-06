void pdu_free(V9fsPDU *pdu)

{

    if (pdu) {

        V9fsState *s = pdu->s;

        

        if (!pdu->cancelled) {

            QLIST_REMOVE(pdu, next);

            QLIST_INSERT_HEAD(&s->free_list, pdu, next);

        }

    }

}