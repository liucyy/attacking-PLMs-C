static void omap2_inth_write(void *opaque, target_phys_addr_t addr,

                             uint64_t value, unsigned size)

{

    struct omap_intr_handler_s *s = (struct omap_intr_handler_s *) opaque;

    int offset = addr;

    int bank_no, line_no;

    struct omap_intr_handler_bank_s *bank = NULL;



    if ((offset & 0xf80) == 0x80) {

        bank_no = (offset & 0x60) >> 5;

        if (bank_no < s->nbanks) {

            offset &= ~0x60;

            bank = &s->bank[bank_no];




        }

    }



    switch (offset) {

    case 0x10:	

        s->autoidle &= 4;

        s->autoidle |= (value & 1) << 2;

        if (value & 2)						

            omap_inth_reset(&s->busdev.qdev);




    case 0x48:	

        s->mask = (value & 4) ? 0 : ~0;				

        if (value & 2) {					

            qemu_set_irq(s->parent_intr[1], 0);

            s->new_agr[1] = ~0;

            omap_inth_update(s, 1);

        }

        if (value & 1) {					

            qemu_set_irq(s->parent_intr[0], 0);

            s->new_agr[0] = ~0;

            omap_inth_update(s, 0);

        }




    case 0x4c:	

        

        if (value & 1)

            fprintf(stderr, "%s: protection mode enable attempt\n",

                            __FUNCTION__);




    case 0x50:	

        s->autoidle &= ~3;

        s->autoidle |= value & 3;




    

    case 0x84:	

        bank->mask = value;

        omap_inth_update(s, 0);

        omap_inth_update(s, 1);




    case 0x88:	

        bank->mask &= ~value;

        omap_inth_update(s, 0);

        omap_inth_update(s, 1);




    case 0x8c:	

        bank->mask |= value;




    case 0x90:	

        bank->irqs |= bank->swi |= value;

        omap_inth_update(s, 0);

        omap_inth_update(s, 1);




    case 0x94:	

        bank->swi &= ~value;

        bank->irqs = bank->swi & bank->inputs;




    

    case 0x100 ... 0x300:	

        bank_no = (offset - 0x100) >> 7;

        if (bank_no > s->nbanks)

            break;

        bank = &s->bank[bank_no];

        line_no = (offset & 0x7f) >> 2;

        bank->priority[line_no] = (value >> 2) & 0x3f;

        bank->fiq &= ~(1 << line_no);

        bank->fiq |= (value & 1) << line_no;




    case 0x00:	

    case 0x14:	

    case 0x40:	

    case 0x44:	

    case 0x80:	

    case 0x98:	

    case 0x9c:	

        OMAP_RO_REG(addr);


    }


}