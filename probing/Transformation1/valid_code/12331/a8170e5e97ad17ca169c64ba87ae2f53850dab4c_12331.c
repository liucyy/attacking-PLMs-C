static void omap_tcmi_write(void *opaque, target_phys_addr_t addr,

                            uint64_t value, unsigned size)

{

    struct omap_mpu_state_s *s = (struct omap_mpu_state_s *) opaque;



    if (size != 4) {

        return omap_badwidth_write32(opaque, addr, value);

    }



    switch (addr) {

    case 0x00:	

    case 0x04:	

    case 0x08:	

    case 0x10:	

    case 0x14:	

    case 0x18:	

    case 0x1c:	

    case 0x20:	

    case 0x24:	

    case 0x28:	

    case 0x2c:	

    case 0x30:	

    case 0x3c:	

    case 0x40:	

        s->tcmi_regs[addr >> 2] = value;

        break;

    case 0x0c:	

        s->tcmi_regs[addr >> 2] = (value & 0xf) | (1 << 4);

        break;



    default:

        OMAP_BAD_REG(addr);

    }

}