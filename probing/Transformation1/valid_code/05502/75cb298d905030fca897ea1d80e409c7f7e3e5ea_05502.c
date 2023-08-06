sbappend(struct socket *so, struct mbuf *m)

{

	int ret = 0;



	DEBUG_CALL("sbappend");

	DEBUG_ARG("so = %p", so);

	DEBUG_ARG("m = %p", m);

	DEBUG_ARG("m->m_len = %d", m->m_len);



	

	if (m->m_len <= 0) {

		m_free(m);

		return;

	}



	

	if (so->so_urgc) {

		sbappendsb(&so->so_rcv, m);

		m_free(m);

		sosendoob(so);

		return;

	}



	

	if (!so->so_rcv.sb_cc)

	   ret = slirp_send(so, m->m_data, m->m_len, 0);



	if (ret <= 0) {

		

		sbappendsb(&so->so_rcv, m);

	} else if (ret != m->m_len) {

		

		m->m_len -= ret;

		m->m_data += ret;

		sbappendsb(&so->so_rcv, m);

	} 

	

	m_free(m);

}