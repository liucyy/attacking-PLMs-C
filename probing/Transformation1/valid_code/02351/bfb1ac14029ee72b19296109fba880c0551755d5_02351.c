void slirp_pollfds_poll(GArray *pollfds, int select_error)

{

    Slirp *slirp;

    struct socket *so, *so_next;

    int ret;



    if (QTAILQ_EMPTY(&slirp_instances)) {

        return;

    }



    curtime = qemu_clock_get_ms(QEMU_CLOCK_REALTIME);



    QTAILQ_FOREACH(slirp, &slirp_instances, entry) {

        

        if (slirp->time_fasttimo &&

            ((curtime - slirp->time_fasttimo) >= TIMEOUT_FAST)) {

            tcp_fasttimo(slirp);

            slirp->time_fasttimo = 0;

        }

        if (slirp->do_slowtimo &&

            ((curtime - slirp->last_slowtimo) >= TIMEOUT_SLOW)) {

            ip_slowtimo(slirp);

            tcp_slowtimo(slirp);

            slirp->last_slowtimo = curtime;

        }



        

        if (!select_error) {

            

            for (so = slirp->tcb.so_next; so != &slirp->tcb;

                    so = so_next) {

                int revents;



                so_next = so->so_next;



                revents = 0;

                if (so->pollfds_idx != -1) {

                    revents = g_array_index(pollfds, GPollFD,

                                            so->pollfds_idx).revents;

                }



                if (so->so_state & SS_NOFDREF || so->s == -1) {

                    continue;

                }



                

                if (revents & G_IO_PRI) {

                    sorecvoob(so);

                }

                

                else if (revents & (G_IO_IN | G_IO_HUP | G_IO_ERR)) {

                    

                    if (so->so_state & SS_FACCEPTCONN) {

                        tcp_connect(so);

                        continue;

                    } 

                    ret = soread(so);



                    

                    if (ret > 0) {

                        tcp_output(sototcpcb(so));

                    }

                }



                

                if (!(so->so_state & SS_NOFDREF) &&

                        (revents & (G_IO_OUT | G_IO_ERR))) {

                    

                    if (so->so_state & SS_ISFCONNECTING) {

                        

                        so->so_state &= ~SS_ISFCONNECTING;



                        ret = send(so->s, (const void *) &ret, 0, 0);

                        if (ret < 0) {

                            

                            if (errno == EAGAIN || errno == EWOULDBLOCK ||

                                errno == EINPROGRESS || errno == ENOTCONN) {

                                continue;

                            }



                            

                            so->so_state &= SS_PERSISTENT_MASK;

                            so->so_state |= SS_NOFDREF;

                        }

                        



                        

                        tcp_input((struct mbuf *)NULL, sizeof(struct ip), so,

                                  so->so_ffamily);

                        

                    } else {

                        ret = sowrite(so);

                    }

                    

                }



                

#ifdef PROBE_CONN

                if (so->so_state & SS_ISFCONNECTING) {

                    ret = qemu_recv(so->s, &ret, 0, 0);



                    if (ret < 0) {

                        

                        if (errno == EAGAIN || errno == EWOULDBLOCK ||

                            errno == EINPROGRESS || errno == ENOTCONN) {

                            continue; 

                        }



                        

                        so->so_state &= SS_PERSISTENT_MASK;

                        so->so_state |= SS_NOFDREF;



                        

                    } else {

                        ret = send(so->s, &ret, 0, 0);

                        if (ret < 0) {

                            

                            if (errno == EAGAIN || errno == EWOULDBLOCK ||

                                errno == EINPROGRESS || errno == ENOTCONN) {

                                continue;

                            }

                            

                            so->so_state &= SS_PERSISTENT_MASK;

                            so->so_state |= SS_NOFDREF;

                        } else {

                            so->so_state &= ~SS_ISFCONNECTING;

                        }



                    }

                    tcp_input((struct mbuf *)NULL, sizeof(struct ip), so,

                              so->so_ffamily);

                } 

#endif

            }



            

            for (so = slirp->udb.so_next; so != &slirp->udb;

                    so = so_next) {

                int revents;



                so_next = so->so_next;



                revents = 0;

                if (so->pollfds_idx != -1) {

                    revents = g_array_index(pollfds, GPollFD,

                            so->pollfds_idx).revents;

                }



                if (so->s != -1 &&

                    (revents & (G_IO_IN | G_IO_HUP | G_IO_ERR))) {

                    sorecvfrom(so);

                }

            }



            

            for (so = slirp->icmp.so_next; so != &slirp->icmp;

                    so = so_next) {

                    int revents;



                    so_next = so->so_next;



                    revents = 0;

                    if (so->pollfds_idx != -1) {

                        revents = g_array_index(pollfds, GPollFD,

                                                so->pollfds_idx).revents;

                    }



                    if (so->s != -1 &&

                        (revents & (G_IO_IN | G_IO_HUP | G_IO_ERR))) {

                    icmp_receive(so);

                }

            }

        }



        if_start(slirp);

    }

}