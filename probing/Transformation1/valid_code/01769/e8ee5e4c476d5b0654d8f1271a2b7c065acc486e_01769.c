static void run_dependent_requests(BDRVQcowState *s, QCowL2Meta *m)

{

    

    if (m->nb_clusters != 0) {

        QLIST_REMOVE(m, next_in_flight);

    }



    

    if (!qemu_co_queue_empty(&m->dependent_requests)) {

        qemu_co_mutex_unlock(&s->lock);

        while(qemu_co_queue_next(&m->dependent_requests));

        qemu_co_mutex_lock(&s->lock);

    }

}