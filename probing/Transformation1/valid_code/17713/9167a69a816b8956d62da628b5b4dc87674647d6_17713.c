static uint8_t lsi_reg_readb(LSIState *s, int offset)

{

    uint8_t tmp;

#define CASE_GET_REG32(name, addr) \

    case addr: return s->name & 0xff; \

    case addr + 1: return (s->name >> 8) & 0xff; \

    case addr + 2: return (s->name >> 16) & 0xff; \

    case addr + 3: return (s->name >> 24) & 0xff;



#ifdef DEBUG_LSI_REG

    DPRINTF("Read reg %x\n", offset);

#endif

    switch (offset) {

    case 0x00: 

        return s->scntl0;

    case 0x01: 

        return s->scntl1;

    case 0x02: 

        return s->scntl2;

    case 0x03: 

        return s->scntl3;

    case 0x04: 

        return s->scid;

    case 0x05: 

        return s->sxfer;

    case 0x06: 

        return s->sdid;

    case 0x07: 

        return 0x7f;

    case 0x08: 

        return 0x00;

    case 0xa: 

        return s->ssid;

    case 0xb: 

        

        return 0;

    case 0xc: 

        tmp = s->dstat | 0x80;

        if ((s->istat0 & LSI_ISTAT0_INTF) == 0)

            s->dstat = 0;

        lsi_update_irq(s);

        return tmp;

    case 0x0d: 

        return s->sstat0;

    case 0x0e: 

        return s->sstat1;

    case 0x0f: 

        return s->scntl1 & LSI_SCNTL1_CON ? 0 : 2;

    CASE_GET_REG32(dsa, 0x10)

    case 0x14: 

        return s->istat0;

    case 0x16: 

        return s->mbox0;

    case 0x17: 

        return s->mbox1;

    case 0x18: 

        return 0xff;

    case 0x19: 

        return 0;

    case 0x1a: 

        tmp = LSI_CTEST2_DACK | LSI_CTEST2_CM;

        if (s->istat0 & LSI_ISTAT0_SIGP) {

            s->istat0 &= ~LSI_ISTAT0_SIGP;

            tmp |= LSI_CTEST2_SIGP;

        }

        return tmp;

    case 0x1b: 

        return s->ctest3;

    CASE_GET_REG32(temp, 0x1c)

    case 0x20: 

        return 0;

    case 0x21: 

        return s->ctest4;

    case 0x22: 

        return s->ctest5;

    case 0x23: 

         return 0;

    case 0x24: 

        return s->dbc & 0xff;

    case 0x25: 

        return (s->dbc >> 8) & 0xff;

    case 0x26: 

        return (s->dbc >> 16) & 0xff;

    case 0x27: 

        return s->dcmd;

    CASE_GET_REG32(dsp, 0x2c)

    CASE_GET_REG32(dsps, 0x30)

    CASE_GET_REG32(scratch[0], 0x34)

    case 0x38: 

        return s->dmode;

    case 0x39: 

        return s->dien;

    case 0x3b: 

        return s->dcntl;

    case 0x40: 

        return s->sien0;

    case 0x41: 

        return s->sien1;

    case 0x42: 

        tmp = s->sist0;

        s->sist0 = 0;

        lsi_update_irq(s);

        return tmp;

    case 0x43: 

        tmp = s->sist1;

        s->sist1 = 0;

        lsi_update_irq(s);

        return tmp;

    case 0x47: 

        return 0x0f;

    case 0x48: 

        return s->stime0;

    case 0x4a: 

        return s->respid0;

    case 0x4b: 

        return s->respid1;

    case 0x4d: 

        return s->stest1;

    case 0x4e: 

        return s->stest2;

    case 0x4f: 

        return s->stest3;

    case 0x50: 

        

        return s->sidl;

    case 0x52: 

        return 0xe0;

    case 0x56: 

        return s->ccntl0;

    case 0x57: 

        return s->ccntl1;

    case 0x58: 

        

        if ((s->sstat1 & PHASE_MASK) == PHASE_MI)

            return s->msg[0];

        return 0;

    case 0x59: 

        return 0;

    CASE_GET_REG32(mmrs, 0xa0)

    CASE_GET_REG32(mmws, 0xa4)

    CASE_GET_REG32(sfs, 0xa8)

    CASE_GET_REG32(drs, 0xac)

    CASE_GET_REG32(sbms, 0xb0)

    CASE_GET_REG32(dmbs, 0xb4)

    CASE_GET_REG32(dnad64, 0xb8)

    CASE_GET_REG32(pmjad1, 0xc0)

    CASE_GET_REG32(pmjad2, 0xc4)

    CASE_GET_REG32(rbc, 0xc8)

    CASE_GET_REG32(ua, 0xcc)

    CASE_GET_REG32(ia, 0xd4)

    CASE_GET_REG32(sbc, 0xd8)

    CASE_GET_REG32(csbc, 0xdc)

    }

    if (offset >= 0x5c && offset < 0xa0) {

        int n;

        int shift;

        n = (offset - 0x58) >> 2;

        shift = (offset & 3) * 8;

        return (s->scratch[n] >> shift) & 0xff;

    }

    BADF("readb 0x%x\n", offset);

    exit(1);

#undef CASE_GET_REG32

}