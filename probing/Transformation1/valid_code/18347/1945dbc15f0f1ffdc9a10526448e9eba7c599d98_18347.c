static void openpic_update_irq(openpic_t *opp, int n_IRQ)

{

    IRQ_src_t *src;

    int i;



    src = &opp->src[n_IRQ];



    if (!src->pending) {

        

        DPRINTF("%s: IRQ %d is not pending\n", __func__, n_IRQ);

        return;

    }

    if (test_bit(&src->ipvp, IPVP_MASK)) {

        

        DPRINTF("%s: IRQ %d is disabled\n", __func__, n_IRQ);

        return;

    }

    if (IPVP_PRIORITY(src->ipvp) == 0) {

        

        DPRINTF("%s: IRQ %d has 0 priority\n", __func__, n_IRQ);

        return;

    }

    if (test_bit(&src->ipvp, IPVP_ACTIVITY)) {

        

        DPRINTF("%s: IRQ %d is already active\n", __func__, n_IRQ);

        return;

    }

    if (src->ide == 0x00000000) {

        

        DPRINTF("%s: IRQ %d has no target\n", __func__, n_IRQ);

        return;

    }



    if (src->ide == (1 << src->last_cpu)) {

        

        IRQ_local_pipe(opp, src->last_cpu, n_IRQ);

    } else if (!test_bit(&src->ipvp, IPVP_MODE)) {

        

        for (i = 0; i < opp->nb_cpus; i++) {

            if (test_bit(&src->ide, i))

                IRQ_local_pipe(opp, i, n_IRQ);

        }

    } else {

        

        for (i = src->last_cpu + 1; i != src->last_cpu; i++) {

            if (i == opp->nb_cpus)

                i = 0;

            if (test_bit(&src->ide, i)) {

                IRQ_local_pipe(opp, i, n_IRQ);

                src->last_cpu = i;

                break;

            }

        }

    }

}