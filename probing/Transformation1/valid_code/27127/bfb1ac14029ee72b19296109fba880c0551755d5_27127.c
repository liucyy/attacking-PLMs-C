sorecvoob(struct socket *so)

{

	struct tcpcb *tp = sototcpcb(so);



	DEBUG_CALL("sorecvoob");

	DEBUG_ARG("so = %p", so);



	

	soread(so);

	tp->snd_up = tp->snd_una + so->so_snd.sb_cc;

	tp->t_force = 1;

	tcp_output(tp);

	tp->t_force = 0;

}