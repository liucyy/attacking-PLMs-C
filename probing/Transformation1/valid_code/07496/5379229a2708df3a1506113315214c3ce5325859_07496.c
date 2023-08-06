icmp_input(struct mbuf *m, int hlen)

{

  register struct icmp *icp;

  register struct ip *ip=mtod(m, struct ip *);

  int icmplen=ip->ip_len;

  Slirp *slirp = m->slirp;



  DEBUG_CALL("icmp_input");

  DEBUG_ARG("m = %p", m);

  DEBUG_ARG("m_len = %d", m->m_len);



  

  if (icmplen < ICMP_MINLEN) {          

  freeit:

    m_free(m);

    goto end_error;

  }



  m->m_len -= hlen;

  m->m_data += hlen;

  icp = mtod(m, struct icmp *);

  if (cksum(m, icmplen)) {

    goto freeit;

  }

  m->m_len += hlen;

  m->m_data -= hlen;



  DEBUG_ARG("icmp_type = %d", icp->icmp_type);

  switch (icp->icmp_type) {

  case ICMP_ECHO:

    ip->ip_len += hlen;	             

    if (ip->ip_dst.s_addr == slirp->vhost_addr.s_addr) {

      icmp_reflect(m);

    } else if (slirp->restricted) {

        goto freeit;

    } else {

      struct socket *so;

      struct sockaddr_in addr;

      if ((so = socreate(slirp)) == NULL) goto freeit;

      if (icmp_send(so, m, hlen) == 0) {

        return;

      }

      if(udp_attach(so) == -1) {

	DEBUG_MISC((dfd,"icmp_input udp_attach errno = %d-%s\n",

		    errno,strerror(errno)));

	sofree(so);

	m_free(m);

	goto end_error;

      }

      so->so_m = m;

      so->so_ffamily = AF_INET;

      so->so_faddr = ip->ip_dst;

      so->so_fport = htons(7);

      so->so_lfamily = AF_INET;

      so->so_laddr = ip->ip_src;

      so->so_lport = htons(9);

      so->so_iptos = ip->ip_tos;

      so->so_type = IPPROTO_ICMP;

      so->so_state = SS_ISFCONNECTED;



      

      addr.sin_family = AF_INET;

      if ((so->so_faddr.s_addr & slirp->vnetwork_mask.s_addr) ==

          slirp->vnetwork_addr.s_addr) {

	

	if (so->so_faddr.s_addr == slirp->vnameserver_addr.s_addr) {

	  if (get_dns_addr(&addr.sin_addr) < 0)

	    addr.sin_addr = loopback_addr;

	} else {

	  addr.sin_addr = loopback_addr;

	}

      } else {

	addr.sin_addr = so->so_faddr;

      }

      addr.sin_port = so->so_fport;

      if(sendto(so->s, icmp_ping_msg, strlen(icmp_ping_msg), 0,

		(struct sockaddr *)&addr, sizeof(addr)) == -1) {

	DEBUG_MISC((dfd,"icmp_input udp sendto tx errno = %d-%s\n",

		    errno,strerror(errno)));

	icmp_error(m, ICMP_UNREACH,ICMP_UNREACH_NET, 0,strerror(errno));

	udp_detach(so);

      }

    } 

    break;

  case ICMP_UNREACH:

    

  case ICMP_TIMXCEED:

  case ICMP_PARAMPROB:

  case ICMP_SOURCEQUENCH:

  case ICMP_TSTAMP:

  case ICMP_MASKREQ:

  case ICMP_REDIRECT:

    m_free(m);

    break;



  default:

    m_free(m);

  } 



end_error:

  

  return;

}