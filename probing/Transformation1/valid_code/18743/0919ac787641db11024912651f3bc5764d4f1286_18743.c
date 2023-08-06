static uint64_t omap2_inth_read(void *opaque, target_phys_addr_t addr,

                                unsigned size)

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

    case 0x00:	

        return 0x21;



    case 0x10:	

        return (s->autoidle >> 2) & 1;



    case 0x14:	

        return 1;						



    case 0x40:	

        return s->sir_intr[0];



    case 0x44:	

        return s->sir_intr[1];



    case 0x48:	

        return (!s->mask) << 2;					



    case 0x4c:	

        return 0;



    case 0x50:	

        return s->autoidle & 3;



    

    case 0x80:	

        return bank->inputs;



    case 0x84:	

        return bank->mask;



    case 0x88:	

    case 0x8c:	

        return 0;



    case 0x90:	

        return bank->swi;



    case 0x94:	

        return 0;



    case 0x98:	

        return bank->irqs & ~bank->mask & ~bank->fiq;



    case 0x9c:	

        return bank->irqs & ~bank->mask & bank->fiq;



    

    case 0x100 ... 0x300:	

        bank_no = (offset - 0x100) >> 7;

        if (bank_no > s->nbanks)

            break;

        bank = &s->bank[bank_no];

        line_no = (offset & 0x7f) >> 2;

        return (bank->priority[line_no] << 2) |

                ((bank->fiq >> line_no) & 1);

    }

    OMAP_BAD_REG(addr);

    return 0;

}