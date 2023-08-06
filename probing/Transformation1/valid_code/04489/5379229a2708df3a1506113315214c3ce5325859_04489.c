sorecvfrom(struct socket *so)

{

	struct sockaddr_storage addr;

	socklen_t addrlen = sizeof(struct sockaddr_storage);



	DEBUG_CALL("sorecvfrom");

	DEBUG_ARG("so = %p", so);



	if (so->so_type == IPPROTO_ICMP) {   

	  char buff[256];

	  int len;



	  len = recvfrom(so->s, buff, 256, 0,

			 (struct sockaddr *)&addr, &addrlen);

	  



	  if(len == -1 || len == 0) {

	    u_char code=ICMP_UNREACH_PORT;



	    if(errno == EHOSTUNREACH) code=ICMP_UNREACH_HOST;

	    else if(errno == ENETUNREACH) code=ICMP_UNREACH_NET;



	    DEBUG_MISC((dfd," udp icmp rx errno = %d-%s\n",

			errno,strerror(errno)));

	    icmp_error(so->so_m, ICMP_UNREACH,code, 0,strerror(errno));

	  } else {

	    icmp_reflect(so->so_m);

            so->so_m = NULL; 

	  }

	  

	  udp_detach(so);

	} else {                            	

	  struct mbuf *m;

          int len;

#ifdef _WIN32

          unsigned long n;

#else

          int n;

#endif



	  m = m_get(so->slirp);

	  if (!m) {

	      return;

	  }

	  m->m_data += IF_MAXLINKHDR;



	  

	  len = M_FREEROOM(m);

	  

	  ioctlsocket(so->s, FIONREAD, &n);



	  if (n > len) {

	    n = (m->m_data - m->m_dat) + m->m_len + n + 1;

	    m_inc(m, n);

	    len = M_FREEROOM(m);

	  }

	  



	  m->m_len = recvfrom(so->s, m->m_data, len, 0,

			      (struct sockaddr *)&addr, &addrlen);

	  DEBUG_MISC((dfd, " did recvfrom %d, errno = %d-%s\n",

		      m->m_len, errno,strerror(errno)));

	  if(m->m_len<0) {

	    u_char code=ICMP_UNREACH_PORT;



	    if(errno == EHOSTUNREACH) code=ICMP_UNREACH_HOST;

	    else if(errno == ENETUNREACH) code=ICMP_UNREACH_NET;



	    DEBUG_MISC((dfd," rx error, tx icmp ICMP_UNREACH:%i\n", code));

	    icmp_error(so->so_m, ICMP_UNREACH,code, 0,strerror(errno));

	    m_free(m);

	  } else {

	  

	    if (so->so_expire) {

	      if (so->so_fport == htons(53))

		so->so_expire = curtime + SO_EXPIREFAST;

	      else

		so->so_expire = curtime + SO_EXPIRE;

	    }



	    

	    switch (so->so_ffamily) {

	    case AF_INET:

	        udp_output(so, m, (struct sockaddr_in *) &addr);

	        break;

	    default:

	        break;

	    }

	  } 

	} 

}