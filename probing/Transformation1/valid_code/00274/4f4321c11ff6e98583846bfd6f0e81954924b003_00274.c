static void musb_rx_req(MUSBState *s, int epnum)

{

    MUSBEndPoint *ep = s->ep + epnum;

    int total;



    

    if (ep->packey[1].p.pid == USB_TOKEN_IN && ep->status[1] >= 0 &&

                    (ep->fifostart[1]) + ep->rxcount <

                    ep->packey[1].p.len) {

        TRACE("0x%08x, %d",  ep->fifostart[1], ep->rxcount );

        ep->fifostart[1] += ep->rxcount;

        ep->fifolen[1] = 0;



        ep->rxcount = MIN(ep->packey[0].p.len - (ep->fifostart[1]),

                        ep->maxp[1]);



        ep->csr[1] &= ~MGC_M_RXCSR_H_REQPKT;

        if (!epnum)

            ep->csr[0] &= ~MGC_M_CSR0_H_REQPKT;



        

        ep->csr[1] &= ~(MGC_M_RXCSR_H_ERROR | MGC_M_RXCSR_H_RXSTALL |

                        MGC_M_RXCSR_DATAERROR);

        if (!epnum)

            ep->csr[0] &= ~(MGC_M_CSR0_H_ERROR | MGC_M_CSR0_H_RXSTALL |

                            MGC_M_CSR0_H_NAKTIMEOUT | MGC_M_CSR0_H_NO_PING);



        ep->csr[1] |= MGC_M_RXCSR_FIFOFULL | MGC_M_RXCSR_RXPKTRDY;

        if (!epnum)

            ep->csr[0] |= MGC_M_CSR0_RXPKTRDY;

        musb_rx_intr_set(s, epnum, 1);

        return;

    }



    

    total = MIN(ep->maxp[1] & 0x3ff, sizeof(s->buf));



#ifdef SETUPLEN_HACK

    

    if (!epnum) {

        if (ep->packey[0].p.devaddr == 2) {

            total = MIN(s->setup_len, 8);

        } else {

            total = MIN(s->setup_len, 64);

        }

        s->setup_len -= total;

    }

#endif



    return musb_packet(s, ep, epnum, USB_TOKEN_IN,

                    total, musb_rx_packet_complete, 1);

}