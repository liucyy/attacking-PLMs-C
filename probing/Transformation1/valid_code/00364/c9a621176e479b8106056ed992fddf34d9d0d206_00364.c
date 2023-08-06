if_start(void)
{
	struct mbuf *ifm, *ifqt;
	DEBUG_CALL("if_start");
	if (if_queued == 0)
	   return; 
 again:
        
        if (!slirp_can_output())
            return;
	
	if (if_fastq.ifq_next != &if_fastq) {
		ifm = if_fastq.ifq_next;
	} else {
		
		if (next_m != &if_batchq)
		   ifm = next_m;
		else
		   ifm = if_batchq.ifq_next;
		
		next_m = ifm->ifq_next;
	}
	
	ifqt = ifm->ifq_prev;
	remque(ifm);
	--if_queued;
	
	if (ifm->ifs_next !=  ifm) {
		insque(ifm->ifs_next, ifqt);
		ifs_remque(ifm);
	}
	
	if (ifm->ifq_so) {
		if (--ifm->ifq_so->so_queued == 0)
		   
		   ifm->ifq_so->so_nqueued = 0;
	}
	
        if_encap(ifm->m_data, ifm->m_len);
	if (if_queued)
	   goto again;
}