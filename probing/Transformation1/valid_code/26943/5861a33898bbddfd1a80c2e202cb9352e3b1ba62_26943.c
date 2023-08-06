static void openpic_cpu_write_internal(void *opaque, hwaddr addr,

                                       uint32_t val, int idx)

{

    openpic_t *opp = opaque;

    IRQ_src_t *src;

    IRQ_dst_t *dst;

    int s_IRQ, n_IRQ;



    DPRINTF("%s: cpu %d addr " TARGET_FMT_plx " <= %08x\n", __func__, idx,

            addr, val);

    if (addr & 0xF)

        return;

    dst = &opp->dst[idx];

    addr &= 0xFF0;

    switch (addr) {

    case 0x40: 

    case 0x50:

    case 0x60:

    case 0x70:

        idx = (addr - 0x40) >> 4;

        

        write_IRQreg_ide(opp, opp->irq_ipi0 + idx,

                         opp->src[opp->irq_ipi0 + idx].ide | val);

        openpic_set_irq(opp, opp->irq_ipi0 + idx, 1);

        openpic_set_irq(opp, opp->irq_ipi0 + idx, 0);

        break;

    case 0x80: 

        dst->pctp = val & 0x0000000F;

        break;

    case 0x90: 

        

        break;

    case 0xA0: 

        

        break;

    case 0xB0: 

        DPRINTF("PEOI\n");

        s_IRQ = IRQ_get_next(opp, &dst->servicing);

        IRQ_resetbit(&dst->servicing, s_IRQ);

        dst->servicing.next = -1;

        

        s_IRQ = IRQ_get_next(opp, &dst->servicing);

        

        n_IRQ = IRQ_get_next(opp, &dst->raised);

        src = &opp->src[n_IRQ];

        if (n_IRQ != -1 &&

            (s_IRQ == -1 ||

             IPVP_PRIORITY(src->ipvp) > dst->servicing.priority)) {

            DPRINTF("Raise OpenPIC INT output cpu %d irq %d\n",

                    idx, n_IRQ);

            opp->irq_raise(opp, idx, src);

        }

        break;

    default:

        break;

    }

}