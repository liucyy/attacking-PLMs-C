static void run_dependent_requests(QCowL2Meta *m)

{

    QCowAIOCB *req;

    QCowAIOCB *next;



    

    if (m->nb_clusters != 0) {

        LIST_REMOVE(m, next_in_flight);

    }



    

    for (req = m->dependent_requests.lh_first; req != NULL; req = next) {

        next = req->next_depend.le_next;

        qcow_aio_write_cb(req, 0);

    }



    

    LIST_INIT(&m->dependent_requests);

}