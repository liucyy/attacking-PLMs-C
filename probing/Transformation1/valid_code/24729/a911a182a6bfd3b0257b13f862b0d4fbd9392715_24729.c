static void put_fid(V9fsPDU *pdu, V9fsFidState *fidp)

{

    BUG_ON(!fidp->ref);

    fidp->ref--;

    

    if (!fidp->ref && fidp->clunked) {

        if (fidp->fid == pdu->s->root_fid) {

            

            if (pdu->s->migration_blocker) {

                migrate_del_blocker(pdu->s->migration_blocker);

                error_free(pdu->s->migration_blocker);

                pdu->s->migration_blocker = NULL;

            }

        }

        free_fid(pdu, fidp);

    }

}