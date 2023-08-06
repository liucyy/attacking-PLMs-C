void slirp_select_poll(fd_set *readfds, fd_set *writefds, fd_set *xfds)

{

    struct socket *so, *so_next;

    int ret;



    global_readfds = readfds;

    global_writefds = writefds;

    global_xfds = xfds;



	

	updtime();



	

	if (link_up) {

		if (time_fasttimo && ((curtime - time_fasttimo) >= 2)) {

			tcp_fasttimo();

			time_fasttimo = 0;

		}

		if (do_slowtimo && ((curtime - last_slowtimo) >= 499)) {

			ip_slowtimo();

			tcp_slowtimo();

			last_slowtimo = curtime;

		}

	}



	

	if (link_up) {

		

		for (so = tcb.so_next; so != &tcb; so = so_next) {

			so_next = so->so_next;



			

			if (so->so_state & SS_NOFDREF || so->s == -1)

			   continue;



			

			if (FD_ISSET(so->s, xfds))

			   sorecvoob(so);

			

			else if (FD_ISSET(so->s, readfds)) {

				

				if (so->so_state & SS_FACCEPTCONN) {

					tcp_connect(so);

					continue;

				} 

				ret = soread(so);



				

				if (ret > 0)

				   tcp_output(sototcpcb(so));

			}



			

			if (FD_ISSET(so->s, writefds)) {

			  

			  if (so->so_state & SS_ISFCONNECTING) {

			    

			    so->so_state &= ~SS_ISFCONNECTING;



			    ret = send(so->s, (const void *) &ret, 0, 0);

			    if (ret < 0) {

			      

			      if (errno == EAGAIN || errno == EWOULDBLOCK ||

				  errno == EINPROGRESS || errno == ENOTCONN)

				continue;



			      

			      so->so_state &= SS_PERSISTENT_MASK;

			      so->so_state |= SS_NOFDREF;

			    }

			    



			    

			    tcp_input((struct mbuf *)NULL, sizeof(struct ip), so);

			    

			  } else

			    ret = sowrite(so);

			  

			}



			

#ifdef PROBE_CONN

			if (so->so_state & SS_ISFCONNECTING) {

			  ret = recv(so->s, (char *)&ret, 0,0);



			  if (ret < 0) {

			    

			    if (errno == EAGAIN || errno == EWOULDBLOCK ||

				errno == EINPROGRESS || errno == ENOTCONN)

			      continue; 



			    

			    so->so_state &= SS_PERSISTENT_MASK;

			    so->so_state |= SS_NOFDREF;



			    

			  } else {

			    ret = send(so->s, &ret, 0,0);

			    if (ret < 0) {

			      

			      if (errno == EAGAIN || errno == EWOULDBLOCK ||

				  errno == EINPROGRESS || errno == ENOTCONN)

				continue;

			      

			      so->so_state &= SS_PERSISTENT_MASK;

			      so->so_state |= SS_NOFDREF;

			    } else

			      so->so_state &= ~SS_ISFCONNECTING;



			  }

			  tcp_input((struct mbuf *)NULL, sizeof(struct ip),so);

			} 

#endif

		}



		

		for (so = udb.so_next; so != &udb; so = so_next) {

			so_next = so->so_next;



			if (so->s != -1 && FD_ISSET(so->s, readfds)) {

                            sorecvfrom(so);

                        }

		}

	}



	

	if (if_queued && link_up)

	   if_start();



	

	 global_readfds = NULL;

	 global_writefds = NULL;

	 global_xfds = NULL;

}