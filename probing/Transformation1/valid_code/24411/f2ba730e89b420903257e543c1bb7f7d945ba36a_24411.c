ip_reass(register struct ip *ip, register struct ipq *fp)

{

	register struct mbuf *m = dtom(ip);

	register struct ipasfrag *q;

	int hlen = ip->ip_hl << 2;

	int i, next;



	DEBUG_CALL("ip_reass");

	DEBUG_ARG("ip = %lx", (long)ip);

	DEBUG_ARG("fp = %lx", (long)fp);

	DEBUG_ARG("m = %lx", (long)m);



	

	m->m_data += hlen;

	m->m_len -= hlen;



	

	if (fp == 0) {

	  struct mbuf *t;

	  if ((t = m_get()) == NULL) goto dropfrag;

	  fp = mtod(t, struct ipq *);

	  insque(&fp->ip_link, &ipq.ip_link);

	  fp->ipq_ttl = IPFRAGTTL;

	  fp->ipq_p = ip->ip_p;

	  fp->ipq_id = ip->ip_id;

	  fp->frag_link.next = fp->frag_link.prev = &fp->frag_link;

	  fp->ipq_src = ip->ip_src;

	  fp->ipq_dst = ip->ip_dst;

	  q = (struct ipasfrag *)fp;

	  goto insert;

	}



	

	for (q = fp->frag_link.next; q != (struct ipasfrag *)&fp->frag_link;

            q = q->ipf_next)

		if (q->ipf_off > ip->ip_off)

			break;



	

	if (q->ipf_prev != &fp->frag_link) {

        struct ipasfrag *pq = q->ipf_prev;

		i = pq->ipf_off + pq->ipf_len - ip->ip_off;

		if (i > 0) {

			if (i >= ip->ip_len)

				goto dropfrag;

			m_adj(dtom(ip), i);

			ip->ip_off += i;

			ip->ip_len -= i;

		}

	}



	

	while (q != (struct ipasfrag*)&fp->frag_link &&

            ip->ip_off + ip->ip_len > q->ipf_off) {

		i = (ip->ip_off + ip->ip_len) - q->ipf_off;

		if (i < q->ipf_len) {

			q->ipf_len -= i;

			q->ipf_off += i;

			m_adj(dtom(q), i);

			break;

		}

		q = q->ipf_next;

		m_freem(dtom(q->ipf_prev));

		ip_deq(q->ipf_prev);

	}



insert:

	

	ip_enq(iptofrag(ip), q->ipf_prev);

	next = 0;

	for (q = fp->frag_link.next; q != (struct ipasfrag*)&fp->frag_link;

            q = q->ipf_next) {

		if (q->ipf_off != next)

			return (0);

		next += q->ipf_len;

	}

	if (((struct ipasfrag *)(q->ipf_prev))->ipf_tos & 1)

		return (0);



	

    q = fp->frag_link.next;

	m = dtom(q);



	q = (struct ipasfrag *) q->ipf_next;

	while (q != (struct ipasfrag*)&fp->frag_link) {

	  struct mbuf *t = dtom(q);

	  q = (struct ipasfrag *) q->ipf_next;

	  m_cat(m, t);

	}



	

	q = fp->frag_link.next;



	

	if (m->m_flags & M_EXT) {

	  int delta;

	  delta = (char *)ip - m->m_dat;

	  q = (struct ipasfrag *)(m->m_ext + delta);

	}



	



    ip = fragtoip(q);

	ip->ip_len = next;

	ip->ip_tos &= ~1;

	ip->ip_src = fp->ipq_src;

	ip->ip_dst = fp->ipq_dst;

	remque(&fp->ip_link);

	(void) m_free(dtom(fp));

	m->m_len += (ip->ip_hl << 2);

	m->m_data -= (ip->ip_hl << 2);



	return ip;



dropfrag:

	STAT(ipstat.ips_fragdropped++);

	m_freem(m);

	return (0);

}