if_output(struct socket *so, struct mbuf *ifm)

{

	struct mbuf *ifq;

	int on_fastq = 1;



	DEBUG_CALL("if_output");

	DEBUG_ARG("so = %lx", (long)so);

	DEBUG_ARG("ifm = %lx", (long)ifm);



	

	if (ifm->m_flags & M_USEDLIST) {

		remque(ifm);

		ifm->m_flags &= ~M_USEDLIST;

	}



	

	for (ifq = if_batchq.ifq_prev; ifq != &if_batchq; ifq = ifq->ifq_prev) {

		if (so == ifq->ifq_so) {

			

			ifm->ifq_so = so;

			ifs_insque(ifm, ifq->ifs_prev);

			goto diddit;

		}

	}



	

	if (so && (so->so_iptos & IPTOS_LOWDELAY)) {

		ifq = if_fastq.ifq_prev;

		on_fastq = 1;

		

		if (ifq->ifq_so == so) {

			ifm->ifq_so = so;

			ifs_insque(ifm, ifq->ifs_prev);

			goto diddit;

		}

	} else

		ifq = if_batchq.ifq_prev;



	

	ifm->ifq_so = so;

	ifs_init(ifm);

	insque(ifm, ifq);



diddit:

	++if_queued;



	if (so) {

		

		so->so_queued++;

		so->so_nqueued++;

		

		if (on_fastq && ((so->so_nqueued >= 6) &&

				 (so->so_nqueued - so->so_queued) >= 3)) {



			

			remque(ifm->ifs_next);



			

			insque(ifm->ifs_next, &if_batchq);

		}

	}



#ifndef FULL_BOLT

	

	if (link_up) {

		

		if_start();

	}

#endif

}