udp_input(register struct mbuf *m, int iphlen)

{

	Slirp *slirp = m->slirp;

	register struct ip *ip;

	register struct udphdr *uh;

	int len;

	struct ip save_ip;

	struct socket *so;



	DEBUG_CALL("udp_input");

	DEBUG_ARG("m = %lx", (long)m);

	DEBUG_ARG("iphlen = %d", iphlen);



	

	if(iphlen > sizeof(struct ip)) {

		ip_stripoptions(m, (struct mbuf *)0);

		iphlen = sizeof(struct ip);

	}



	

	ip = mtod(m, struct ip *);

	uh = (struct udphdr *)((caddr_t)ip + iphlen);



	

	len = ntohs((uint16_t)uh->uh_ulen);



	if (ip->ip_len != len) {

		if (len > ip->ip_len) {

			goto bad;

		}

		m_adj(m, len - ip->ip_len);

		ip->ip_len = len;

	}



	

	save_ip = *ip;

	save_ip.ip_len+= iphlen;         



	

	if (uh->uh_sum) {

      memset(&((struct ipovly *)ip)->ih_mbuf, 0, sizeof(struct mbuf_ptr));

	  ((struct ipovly *)ip)->ih_x1 = 0;

	  ((struct ipovly *)ip)->ih_len = uh->uh_ulen;

	  if(cksum(m, len + sizeof(struct ip))) {

	    goto bad;

	  }

	}



        

        if (ntohs(uh->uh_dport) == BOOTP_SERVER &&

            (ip->ip_dst.s_addr == slirp->vhost_addr.s_addr ||

             ip->ip_dst.s_addr == 0xffffffff)) {

                bootp_input(m);

                goto bad;

            }



        

        if (ntohs(uh->uh_dport) == TFTP_SERVER &&

            ip->ip_dst.s_addr == slirp->vhost_addr.s_addr) {

            tftp_input(m);

            goto bad;

        }



        if (slirp->restricted) {

            goto bad;

        }



	

	so = slirp->udp_last_so;

	if (so->so_lport != uh->uh_sport ||

	    so->so_laddr.s_addr != ip->ip_src.s_addr) {

		struct socket *tmp;



		for (tmp = slirp->udb.so_next; tmp != &slirp->udb;

		     tmp = tmp->so_next) {

			if (tmp->so_lport == uh->uh_sport &&

			    tmp->so_laddr.s_addr == ip->ip_src.s_addr) {

				so = tmp;

				break;

			}

		}

		if (tmp == &slirp->udb) {

		  so = NULL;

		} else {

		  slirp->udp_last_so = so;

		}

	}



	if (so == NULL) {

	  

	  so = socreate(slirp);

	  if (!so) {

	      goto bad;

	  }

	  if(udp_attach(so) == -1) {

	    DEBUG_MISC((dfd," udp_attach errno = %d-%s\n",

			errno,strerror(errno)));

	    sofree(so);

	    goto bad;

	  }



	  

	  so->so_laddr = ip->ip_src;

	  so->so_lport = uh->uh_sport;



	  if ((so->so_iptos = udp_tos(so)) == 0)

	    so->so_iptos = ip->ip_tos;



	  

	}



        so->so_faddr = ip->ip_dst; 

        so->so_fport = uh->uh_dport; 



	iphlen += sizeof(struct udphdr);

	m->m_len -= iphlen;

	m->m_data += iphlen;



	

	if(sosendto(so,m) == -1) {

	  m->m_len += iphlen;

	  m->m_data -= iphlen;

	  *ip=save_ip;

	  DEBUG_MISC((dfd,"udp tx errno = %d-%s\n",errno,strerror(errno)));

	  icmp_error(m, ICMP_UNREACH,ICMP_UNREACH_NET, 0,strerror(errno));

	}



	m_free(so->so_m);   



	

	m->m_len += iphlen;

	m->m_data -= iphlen;

	*ip=save_ip;

	so->so_m=m;         



	return;

bad:

	m_free(m);

}