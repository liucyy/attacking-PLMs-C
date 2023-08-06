void if_start(Slirp *slirp)

{

    uint64_t now = qemu_clock_get_ns(QEMU_CLOCK_REALTIME);

    bool from_batchq, next_from_batchq;

    struct mbuf *ifm, *ifm_next, *ifqt;



    DEBUG_CALL("if_start");



    if (slirp->if_start_busy) {

        return;

    }

    slirp->if_start_busy = true;



    if (slirp->if_fastq.ifq_next != &slirp->if_fastq) {

        ifm_next = slirp->if_fastq.ifq_next;

        next_from_batchq = false;

    } else if (slirp->next_m != &slirp->if_batchq) {

        

        ifm_next = slirp->next_m;

        next_from_batchq = true;

    } else {

        ifm_next = NULL;

    }



    while (ifm_next) {

        ifm = ifm_next;

        from_batchq = next_from_batchq;



        ifm_next = ifm->ifq_next;

        if (ifm_next == &slirp->if_fastq) {

            

            ifm_next = slirp->next_m;

            next_from_batchq = true;

        }

        if (ifm_next == &slirp->if_batchq) {

            

            ifm_next = NULL;

        }



        

        if (ifm->expiration_date >= now && !if_encap(slirp, ifm)) {

            

            continue;

        }



        if (ifm == slirp->next_m) {

            

            slirp->next_m = ifm->ifq_next;

        }



        

        ifqt = ifm->ifq_prev;

        remque(ifm);



        

        if (ifm->ifs_next != ifm) {

            struct mbuf *next = ifm->ifs_next;



            insque(next, ifqt);

            ifs_remque(ifm);



            if (!from_batchq) {

                

                ifm_next = next;

                next_from_batchq = false;

            } else if (slirp->next_m == &slirp->if_batchq) {

                

                slirp->next_m = ifm_next = next;

            }

        }



        

        if (ifm->ifq_so && --ifm->ifq_so->so_queued == 0) {

            

            ifm->ifq_so->so_nqueued = 0;

        }



        m_free(ifm);

    }



    slirp->if_start_busy = false;

}