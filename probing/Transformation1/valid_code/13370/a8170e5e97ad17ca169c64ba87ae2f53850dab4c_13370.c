static uint64_t omap_mpuio_read(void *opaque, target_phys_addr_t addr,

                                unsigned size)

{

    struct omap_mpuio_s *s = (struct omap_mpuio_s *) opaque;

    int offset = addr & OMAP_MPUI_REG_MASK;

    uint16_t ret;



    if (size != 2) {

        return omap_badwidth_read16(opaque, addr);

    }



    switch (offset) {

    case 0x00:	

        return s->inputs;



    case 0x04:	

        return s->outputs;



    case 0x08:	

        return s->dir;



    case 0x10:	

        return s->row_latch;



    case 0x14:	

        return s->cols;



    case 0x18:	

        return s->event;



    case 0x1c:	

        return s->edge;



    case 0x20:	

        return (~s->row_latch & 0x1f) && !s->kbd_mask;



    case 0x24:	

        ret = s->ints;

        s->ints &= s->mask;

        if (ret)

            qemu_irq_lower(s->irq);

        return ret;



    case 0x28:	

        return s->kbd_mask;



    case 0x2c:	

        return s->mask;



    case 0x30:	

        return s->debounce;



    case 0x34:	

        return s->latch;

    }



    OMAP_BAD_REG(addr);

    return 0;

}