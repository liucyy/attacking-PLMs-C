static uint64_t omap_tcmi_read(void *opaque, target_phys_addr_t addr,

                               unsigned size)

{

    struct omap_mpu_state_s *s = (struct omap_mpu_state_s *) opaque;

    uint32_t ret;



    if (size != 4) {

        return omap_badwidth_read32(opaque, addr);

    }



    switch (addr) {

    case 0x00:	

    case 0x04:	

    case 0x08:	

    case 0x0c:	

    case 0x10:	

    case 0x14:	

    case 0x18:	

    case 0x1c:	

    case 0x24:	

    case 0x28:	

    case 0x2c:	

    case 0x30:	

    case 0x3c:	

    case 0x40:	

        return s->tcmi_regs[addr >> 2];



    case 0x20:	

        ret = s->tcmi_regs[addr >> 2];

        s->tcmi_regs[addr >> 2] &= ~1; 

        

        return ret;

    }



    OMAP_BAD_REG(addr);

    return 0;

}