soread(so)

	struct socket *so;

{

	int n, nn, lss, total;

	struct sbuf *sb = &so->so_snd;

	int len = sb->sb_datalen - sb->sb_cc;

	struct iovec iov[2];

	int mss = so->so_tcpcb->t_maxseg;

	

	DEBUG_CALL("soread");

	DEBUG_ARG("so = %lx", (long )so);

	

	

	

	len = sb->sb_datalen - sb->sb_cc;

	

	iov[0].iov_base = sb->sb_wptr;

	if (sb->sb_wptr < sb->sb_rptr) {

		iov[0].iov_len = sb->sb_rptr - sb->sb_wptr;

		

		if (iov[0].iov_len > len)

		   iov[0].iov_len = len;

		if (iov[0].iov_len > mss)

		   iov[0].iov_len -= iov[0].iov_len%mss;

		n = 1;

	} else {

		iov[0].iov_len = (sb->sb_data + sb->sb_datalen) - sb->sb_wptr;

		

		if (iov[0].iov_len > len) iov[0].iov_len = len;

		len -= iov[0].iov_len;

		if (len) {

			iov[1].iov_base = sb->sb_data;

			iov[1].iov_len = sb->sb_rptr - sb->sb_data;

			if(iov[1].iov_len > len)

			   iov[1].iov_len = len;

			total = iov[0].iov_len + iov[1].iov_len;

			if (total > mss) {

				lss = total%mss;

				if (iov[1].iov_len > lss) {

					iov[1].iov_len -= lss;

					n = 2;

				} else {

					lss -= iov[1].iov_len;

					iov[0].iov_len -= lss;

					n = 1;

				}

			} else

				n = 2;

		} else {

			if (iov[0].iov_len > mss)

			   iov[0].iov_len -= iov[0].iov_len%mss;

			n = 1;

		}

	}

	

#ifdef HAVE_READV

	nn = readv(so->s, (struct iovec *)iov, n);

	DEBUG_MISC((dfd, " ... read nn = %d bytes\n", nn));

#else

	nn = recv(so->s, iov[0].iov_base, iov[0].iov_len,0);

#endif	

	if (nn <= 0) {

		if (nn < 0 && (errno == EINTR || errno == EAGAIN))

			return 0;

		else {

			DEBUG_MISC((dfd, " --- soread() disconnected, nn = %d, errno = %d-%s\n", nn, errno,strerror(errno)));

			sofcantrcvmore(so);

			tcp_sockclosed(sototcpcb(so));

			return -1;

		}

	}

	

#ifndef HAVE_READV

	

	if (n == 2 && nn == iov[0].iov_len)

	   nn += recv(so->s, iov[1].iov_base, iov[1].iov_len,0);

	

	DEBUG_MISC((dfd, " ... read nn = %d bytes\n", nn));

#endif

	

	

	sb->sb_cc += nn;

	sb->sb_wptr += nn;

	if (sb->sb_wptr >= (sb->sb_data + sb->sb_datalen))

		sb->sb_wptr -= sb->sb_datalen;

	return nn;

}