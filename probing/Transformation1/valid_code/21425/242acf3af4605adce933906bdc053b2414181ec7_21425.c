udp_emu(struct socket *so, struct mbuf *m)

{

	struct sockaddr_in addr;

        int addrlen = sizeof(addr);

#ifdef EMULATE_TALK

	CTL_MSG_OLD *omsg;

	CTL_MSG *nmsg;

	char buff[sizeof(CTL_MSG)];

	u_char type;



struct talk_request {

	struct talk_request *next;

	struct socket *udp_so;

	struct socket *tcp_so;

} *req;



	static struct talk_request *req_tbl = 0;



#endif



struct cu_header {

	uint16_t	d_family;		

	uint16_t	d_port;			

	uint32_t	d_addr;			

	uint16_t	s_family;		

	uint16_t	s_port;			

	uint32_t	so_addr;		

	uint32_t	seqn;			

	uint16_t	message;		

	uint16_t	data_type;		

	uint16_t	pkt_len;		

} *cu_head;



	switch(so->so_emu) {



#ifdef EMULATE_TALK

	 case EMU_TALK:

	 case EMU_NTALK:

		

		if (getsockname(so->s, (struct sockaddr *)&addr, &addrlen) < 0)

			return;



#define	IS_OLD	(so->so_emu == EMU_TALK)



#define COPY_MSG(dest, src) { dest->type = src->type; \

			      dest->id_num = src->id_num; \

			      dest->pid = src->pid; \

			      dest->addr = src->addr; \

			      dest->ctl_addr = src->ctl_addr; \

			      memcpy(&dest->l_name, &src->l_name, NAME_SIZE_OLD); \

			      memcpy(&dest->r_name, &src->r_name, NAME_SIZE_OLD); \

	         	      memcpy(&dest->r_tty, &src->r_tty, TTY_SIZE); }



#define OTOSIN(ptr, field) ((struct sockaddr_in *)&ptr->field)







		if (IS_OLD) {  		

			omsg = mtod(m, CTL_MSG_OLD*);

			nmsg = (CTL_MSG *) buff;

			type = omsg->type;

			OTOSIN(omsg, ctl_addr)->sin_port = addr.sin_port;

			OTOSIN(omsg, ctl_addr)->sin_addr = our_addr;

			strncpy(omsg->l_name, getlogin(), NAME_SIZE_OLD);

		} else {		

			omsg = (CTL_MSG_OLD *) buff;

			nmsg = mtod(m, CTL_MSG *);

			type = nmsg->type;

			OTOSIN(nmsg, ctl_addr)->sin_port = addr.sin_port;

			OTOSIN(nmsg, ctl_addr)->sin_addr = our_addr;

			strncpy(nmsg->l_name, getlogin(), NAME_SIZE_OLD);

		}



		if (type == LOOK_UP)

			return;		



		if (IS_OLD) {		

			COPY_MSG(nmsg, omsg);

			nmsg->vers = 1;

			nmsg->answer = 0;

		} else

			COPY_MSG(omsg, nmsg);



		



		if (type == ANNOUNCE) {

			int s;

			u_short temp_port;



			for(req = req_tbl; req; req = req->next)

				if (so == req->udp_so)

					break;  	



			if (!req) {	

				req = (struct talk_request *)

					malloc(sizeof(struct talk_request));

				req->udp_so = so;

				req->tcp_so = solisten(0,

					OTOSIN(omsg, addr)->sin_addr.s_addr,

					OTOSIN(omsg, addr)->sin_port,

					SS_FACCEPTONCE);

				req->next = req_tbl;

				req_tbl = req;

			}



			

			addrlen = sizeof(addr);

			getsockname(req->tcp_so->s,

					(struct sockaddr *) &addr,

					&addrlen);

			OTOSIN(omsg, addr)->sin_port = addr.sin_port;

			OTOSIN(omsg, addr)->sin_addr = our_addr;

			OTOSIN(nmsg, addr)->sin_port = addr.sin_port;

			OTOSIN(nmsg, addr)->sin_addr = our_addr;



			

			temp_port = OTOSIN(omsg, ctl_addr)->sin_port;

			OTOSIN(omsg, ctl_addr)->sin_port = 0;

			OTOSIN(nmsg, ctl_addr)->sin_port = 0;

			omsg->type = nmsg->type = LEAVE_INVITE;



			s = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

			addr.sin_addr = our_addr;

			addr.sin_family = AF_INET;

			addr.sin_port = htons(517);

			sendto(s, (char *)omsg, sizeof(*omsg), 0,

				(struct sockaddr *)&addr, sizeof(addr));

			addr.sin_port = htons(518);

			sendto(s, (char *)nmsg, sizeof(*nmsg), 0,

				(struct sockaddr *) &addr, sizeof(addr));

			closesocket(s) ;



			omsg->type = nmsg->type = ANNOUNCE;

			OTOSIN(omsg, ctl_addr)->sin_port = temp_port;

			OTOSIN(nmsg, ctl_addr)->sin_port = temp_port;

		}



		



		if (type == DELETE) {

			struct talk_request *temp_req, *req_next;

			int s;

			u_short temp_port;



			temp_port = OTOSIN(omsg, ctl_addr)->sin_port;

			OTOSIN(omsg, ctl_addr)->sin_port = 0;

			OTOSIN(nmsg, ctl_addr)->sin_port = 0;



			s = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

			addr.sin_addr = our_addr;

			addr.sin_family = AF_INET;

			addr.sin_port = htons(517);

			sendto(s, (char *)omsg, sizeof(*omsg), 0,

				(struct sockaddr *)&addr, sizeof(addr));

			addr.sin_port = htons(518);

			sendto(s, (char *)nmsg, sizeof(*nmsg), 0,

				(struct sockaddr *)&addr, sizeof(addr));

			closesocket(s);



			OTOSIN(omsg, ctl_addr)->sin_port = temp_port;

			OTOSIN(nmsg, ctl_addr)->sin_port = temp_port;



			

			if (so == req_tbl->udp_so) {

				temp_req = req_tbl;

				req_tbl = req_tbl->next;

				free(temp_req);

			} else {

				temp_req = req_tbl;

				for(req = req_tbl->next; req; req = req_next) {

					req_next = req->next;

					if (so == req->udp_so) {

						temp_req->next = req_next;

						free(req);

						break;

					} else {

						temp_req = req;

					}

				}

			}

		}



		return;

#endif



	case EMU_CUSEEME:



		

		if (m->m_len >= sizeof (*cu_head)) {

			if (getsockname(so->s, (struct sockaddr *)&addr, &addrlen) < 0)

				return;

			cu_head = mtod(m, struct cu_header *);

			cu_head->s_port = addr.sin_port;

			cu_head->so_addr = our_addr.s_addr;

		}



		return;

	}

}