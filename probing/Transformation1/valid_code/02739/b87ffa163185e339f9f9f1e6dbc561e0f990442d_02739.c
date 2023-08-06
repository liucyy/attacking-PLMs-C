if_start(Slirp *slirp)

{

    uint64_t now = qemu_get_clock_ns(rt_clock);

    int requeued = 0;

    bool from_batchq = false;

	struct mbuf *ifm, *ifqt;



	DEBUG_CALL("if_start");



	if (slirp->if_queued == 0)

	   return; 



 again:

        

        if (!slirp_can_output(slirp->opaque))

            return;



	

	if (slirp->if_fastq.ifq_next != &slirp->if_fastq) {

		ifm = slirp->if_fastq.ifq_next;

	} else {

		

		if (slirp->next_m != &slirp->if_batchq)

		   ifm = slirp->next_m;

		else

		   ifm = slirp->if_batchq.ifq_next;



                from_batchq = true;

	}



        slirp->if_queued--;



        

        if (ifm->expiration_date >= now && !if_encap(slirp, ifm)) {

            

            requeued++;

            goto out;

        }



        if (from_batchq) {

            

            slirp->next_m = ifm->ifq_next;

        }



	

	ifqt = ifm->ifq_prev;

	remque(ifm);



	

	if (ifm->ifs_next !=  ifm) {

		insque(ifm->ifs_next, ifqt);

		ifs_remque(ifm);

	}



	

	if (ifm->ifq_so) {

		if (--ifm->ifq_so->so_queued == 0)

		   

		   ifm->ifq_so->so_nqueued = 0;

	}



        m_free(ifm);



 out:

	if (slirp->if_queued)

	   goto again;



        slirp->if_queued = requeued;

}