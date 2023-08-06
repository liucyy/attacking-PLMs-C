void slirp_select_fill(int *pnfds,

                       fd_set *readfds, fd_set *writefds, fd_set *xfds)

{

    Slirp *slirp;

    struct socket *so, *so_next;

    int nfds;



    if (QTAILQ_EMPTY(&slirp_instances)) {

        return;

    }



    

    global_readfds = NULL;

    global_writefds = NULL;

    global_xfds = NULL;



    nfds = *pnfds;

    

    do_slowtimo = 0;



    QTAILQ_FOREACH(slirp, &slirp_instances, entry) {

        

        do_slowtimo |= ((slirp->tcb.so_next != &slirp->tcb) ||

                (&slirp->ipq.ip_link != slirp->ipq.ip_link.next));



        for (so = slirp->tcb.so_next; so != &slirp->tcb;

                so = so_next) {

            so_next = so->so_next;



            

            if (time_fasttimo == 0 && so->so_tcpcb->t_flags & TF_DELACK) {

                time_fasttimo = curtime; 

            }



            

            if (so->so_state & SS_NOFDREF || so->s == -1) {

                continue;

            }



            

            if (so->so_state & SS_FACCEPTCONN) {

                FD_SET(so->s, readfds);

                UPD_NFDS(so->s);

                continue;

            }



            

            if (so->so_state & SS_ISFCONNECTING) {

                FD_SET(so->s, writefds);

                UPD_NFDS(so->s);

                continue;

            }



            

            if (CONN_CANFSEND(so) && so->so_rcv.sb_cc) {

                FD_SET(so->s, writefds);

                UPD_NFDS(so->s);

            }



            

            if (CONN_CANFRCV(so) &&

                (so->so_snd.sb_cc < (so->so_snd.sb_datalen/2))) {

                FD_SET(so->s, readfds);

                FD_SET(so->s, xfds);

                UPD_NFDS(so->s);

            }

        }



        

        for (so = slirp->udb.so_next; so != &slirp->udb;

                so = so_next) {

            so_next = so->so_next;



            

            if (so->so_expire) {

                if (so->so_expire <= curtime) {

                    udp_detach(so);

                    continue;

                } else {

                    do_slowtimo = 1; 

                }

            }



            

            if ((so->so_state & SS_ISFCONNECTED) && so->so_queued <= 4) {

                FD_SET(so->s, readfds);

                UPD_NFDS(so->s);

            }

        }



        

        for (so = slirp->icmp.so_next; so != &slirp->icmp;

                so = so_next) {

            so_next = so->so_next;



            

            if (so->so_expire) {

                if (so->so_expire <= curtime) {

                    icmp_detach(so);

                    continue;

                } else {

                    do_slowtimo = 1; 

                }

            }



            if (so->so_state & SS_ISFCONNECTED) {

                FD_SET(so->s, readfds);

                UPD_NFDS(so->s);

            }

        }

    }



    *pnfds = nfds;

}