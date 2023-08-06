sofree(struct socket *so)

{

  Slirp *slirp = so->slirp;

  struct mbuf *ifm;



  for (ifm = (struct mbuf *) slirp->if_fastq.qh_link;

       (struct quehead *) ifm != &slirp->if_fastq;

       ifm = ifm->ifq_next) {

    if (ifm->ifq_so == so) {

      ifm->ifq_so = NULL;





  for (ifm = (struct mbuf *) slirp->if_batchq.qh_link;

       (struct quehead *) ifm != &slirp->if_batchq;

       ifm = ifm->ifq_next) {

    if (ifm->ifq_so == so) {

      ifm->ifq_so = NULL;





  if (so->so_emu==EMU_RSH && so->extra) {

	sofree(so->extra);

	so->extra=NULL;


  if (so == slirp->tcp_last_so) {

      slirp->tcp_last_so = &slirp->tcb;

  } else if (so == slirp->udp_last_so) {

      slirp->udp_last_so = &slirp->udb;

  } else if (so == slirp->icmp_last_so) {

      slirp->icmp_last_so = &slirp->icmp;


  m_free(so->so_m);



  if(so->so_next && so->so_prev)

    remque(so);  






  free(so);