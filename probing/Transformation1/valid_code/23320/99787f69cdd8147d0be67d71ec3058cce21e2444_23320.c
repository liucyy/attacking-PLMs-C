ip_input(struct mbuf *m)

{

	Slirp *slirp = m->slirp;

	register struct ip *ip;

	int hlen;



	DEBUG_CALL("ip_input");

	DEBUG_ARG("m = %p", m);

	DEBUG_ARG("m_len = %d", m->m_len);



	if (m->m_len < sizeof (struct ip)) {

		return;

	}



	ip = mtod(m, struct ip *);



	if (ip->ip_v != IPVERSION) {

		goto bad;

	}



	hlen = ip->ip_hl << 2;

	if (hlen<sizeof(struct ip ) || hlen>m->m_len) {

	  goto bad;                                  

	}



        

	if(cksum(m,hlen)) {

	  goto bad;

	}



	

	NTOHS(ip->ip_len);

	if (ip->ip_len < hlen) {

		goto bad;

	}

	NTOHS(ip->ip_id);

	NTOHS(ip->ip_off);



	

	if (m->m_len < ip->ip_len) {

		goto bad;

	}



	

	if (m->m_len > ip->ip_len)

	   m_adj(m, ip->ip_len - m->m_len);



	

	if (ip->ip_ttl == 0) {

	    icmp_send_error(m, ICMP_TIMXCEED, ICMP_TIMXCEED_INTRANS, 0, "ttl");

	    goto bad;

	}



	

	if (ip->ip_off &~ IP_DF) {

	  register struct ipq *fp;

      struct qlink *l;

		

		for (l = slirp->ipq.ip_link.next; l != &slirp->ipq.ip_link;

		     l = l->next) {

            fp = container_of(l, struct ipq, ip_link);

            if (ip->ip_id == fp->ipq_id &&

                    ip->ip_src.s_addr == fp->ipq_src.s_addr &&

                    ip->ip_dst.s_addr == fp->ipq_dst.s_addr &&

                    ip->ip_p == fp->ipq_p)

		    goto found;

        }

        fp = NULL;

	found:



		

		ip->ip_len -= hlen;

		if (ip->ip_off & IP_MF)

		  ip->ip_tos |= 1;

		else

		  ip->ip_tos &= ~1;



		ip->ip_off <<= 3;



		

		if (ip->ip_tos & 1 || ip->ip_off) {

			ip = ip_reass(slirp, ip, fp);

                        if (ip == NULL)

				return;

			m = dtom(slirp, ip);

		} else

			if (fp)

		   	   ip_freef(slirp, fp);



	} else

		ip->ip_len -= hlen;



	

	switch (ip->ip_p) {

	 case IPPROTO_TCP:

		tcp_input(m, hlen, (struct socket *)NULL, AF_INET);

		break;

	 case IPPROTO_UDP:

		udp_input(m, hlen);

		break;

	 case IPPROTO_ICMP:

		icmp_input(m, hlen);

		break;

	 default:

		m_free(m);

	}

	return;

bad:

	m_free(m);

}