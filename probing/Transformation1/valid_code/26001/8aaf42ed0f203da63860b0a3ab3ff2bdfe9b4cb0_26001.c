static void dhcp_decode(const struct bootp_t *bp, int *pmsg_type,

                        const struct in_addr **preq_addr)

{

    const uint8_t *p, *p_end;

    int len, tag;



    *pmsg_type = 0;

    *preq_addr = NULL;



    p = bp->bp_vend;

    p_end = p + DHCP_OPT_LEN;

    if (memcmp(p, rfc1533_cookie, 4) != 0)

        return;

    p += 4;

    while (p < p_end) {

        tag = p[0];

        if (tag == RFC1533_PAD) {

            p++;

        } else if (tag == RFC1533_END) {

            break;

        } else {

            p++;

            if (p >= p_end)

                break;

            len = *p++;

            DPRINTF("dhcp: tag=%d len=%d\n", tag, len);



            switch(tag) {

            case RFC2132_MSG_TYPE:

                if (len >= 1)

                    *pmsg_type = p[0];

                break;

            case RFC2132_REQ_ADDR:

                if (len >= 4)

                    *preq_addr = (struct in_addr *)p;

                break;

            default:

                break;

            }

            p += len;

        }

    }

    if (*pmsg_type == DHCPREQUEST && !*preq_addr && bp->bp_ciaddr.s_addr) {

        *preq_addr = &bp->bp_ciaddr;

    }

}