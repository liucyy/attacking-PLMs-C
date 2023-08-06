static uint32_t nvic_readl(NVICState *s, uint32_t offset, MemTxAttrs attrs)

{

    ARMCPU *cpu = s->cpu;

    uint32_t val;



    switch (offset) {

    case 4: 

        return ((s->num_irq - NVIC_FIRST_IRQ) / 32) - 1;

    case 0xd00: 

        return cpu->midr;

    case 0xd04: 

        

        val = cpu->env.v7m.exception;

        

        val |= (s->vectpending & 0xff) << 12;

        

        if (nvic_isrpending(s)) {

            val |= (1 << 22);

        }

        

        if (nvic_rettobase(s)) {

            val |= (1 << 11);

        }

        

        if (s->vectors[ARMV7M_EXCP_SYSTICK].pending) {

            val |= (1 << 26);

        }

        

        if (s->vectors[ARMV7M_EXCP_PENDSV].pending) {

            val |= (1 << 28);

        }

        

        if (s->vectors[ARMV7M_EXCP_NMI].pending) {

            val |= (1 << 31);

        }

        

        return val;

    case 0xd08: 

        return cpu->env.v7m.vecbase[attrs.secure];

    case 0xd0c: 

        return 0xfa050000 | (s->prigroup << 8);

    case 0xd10: 

        

        return 0;

    case 0xd14: 

        return cpu->env.v7m.ccr;

    case 0xd24: 

        val = 0;

        if (s->vectors[ARMV7M_EXCP_MEM].active) {

            val |= (1 << 0);

        }

        if (s->vectors[ARMV7M_EXCP_BUS].active) {

            val |= (1 << 1);

        }

        if (s->vectors[ARMV7M_EXCP_USAGE].active) {

            val |= (1 << 3);

        }

        if (s->vectors[ARMV7M_EXCP_SVC].active) {

            val |= (1 << 7);

        }

        if (s->vectors[ARMV7M_EXCP_DEBUG].active) {

            val |= (1 << 8);

        }

        if (s->vectors[ARMV7M_EXCP_PENDSV].active) {

            val |= (1 << 10);

        }

        if (s->vectors[ARMV7M_EXCP_SYSTICK].active) {

            val |= (1 << 11);

        }

        if (s->vectors[ARMV7M_EXCP_USAGE].pending) {

            val |= (1 << 12);

        }

        if (s->vectors[ARMV7M_EXCP_MEM].pending) {

            val |= (1 << 13);

        }

        if (s->vectors[ARMV7M_EXCP_BUS].pending) {

            val |= (1 << 14);

        }

        if (s->vectors[ARMV7M_EXCP_SVC].pending) {

            val |= (1 << 15);

        }

        if (s->vectors[ARMV7M_EXCP_MEM].enabled) {

            val |= (1 << 16);

        }

        if (s->vectors[ARMV7M_EXCP_BUS].enabled) {

            val |= (1 << 17);

        }

        if (s->vectors[ARMV7M_EXCP_USAGE].enabled) {

            val |= (1 << 18);

        }

        return val;

    case 0xd28: 

        return cpu->env.v7m.cfsr;

    case 0xd2c: 

        return cpu->env.v7m.hfsr;

    case 0xd30: 

        return cpu->env.v7m.dfsr;

    case 0xd34: 

        return cpu->env.v7m.mmfar;

    case 0xd38: 

        return cpu->env.v7m.bfar;

    case 0xd3c: 

        

        qemu_log_mask(LOG_UNIMP,

                      "Aux Fault status registers unimplemented\n");

        return 0;

    case 0xd40: 

        return 0x00000030;

    case 0xd44: 

        return 0x00000200;

    case 0xd48: 

        return 0x00100000;

    case 0xd4c: 

        return 0x00000000;

    case 0xd50: 

        return 0x00000030;

    case 0xd54: 

        return 0x00000000;

    case 0xd58: 

        return 0x00000000;

    case 0xd5c: 

        return 0x00000000;

    case 0xd60: 

        return 0x01141110;

    case 0xd64: 

        return 0x02111000;

    case 0xd68: 

        return 0x21112231;

    case 0xd6c: 

        return 0x01111110;

    case 0xd70: 

        return 0x01310102;

    

    case 0xd90: 

        

        return cpu->pmsav7_dregion << 8;

        break;

    case 0xd94: 

        return cpu->env.v7m.mpu_ctrl;

    case 0xd98: 

        return cpu->env.pmsav7.rnr[attrs.secure];

    case 0xd9c: 

    case 0xda4: 

    case 0xdac: 

    case 0xdb4: 

    {

        int region = cpu->env.pmsav7.rnr[attrs.secure];



        if (arm_feature(&cpu->env, ARM_FEATURE_V8)) {

            

            int aliasno = (offset - 0xd9c) / 8; 

            if (aliasno) {

                region = deposit32(region, 0, 2, aliasno);

            }

            if (region >= cpu->pmsav7_dregion) {

                return 0;

            }

            return cpu->env.pmsav8.rbar[attrs.secure][region];

        }



        if (region >= cpu->pmsav7_dregion) {

            return 0;

        }

        return (cpu->env.pmsav7.drbar[region] & 0x1f) | (region & 0xf);

    }

    case 0xda0: 

    case 0xda8: 

    case 0xdb0: 

    case 0xdb8: 

    {

        int region = cpu->env.pmsav7.rnr[attrs.secure];



        if (arm_feature(&cpu->env, ARM_FEATURE_V8)) {

            

            int aliasno = (offset - 0xda0) / 8; 

            if (aliasno) {

                region = deposit32(region, 0, 2, aliasno);

            }

            if (region >= cpu->pmsav7_dregion) {

                return 0;

            }

            return cpu->env.pmsav8.rlar[attrs.secure][region];

        }



        if (region >= cpu->pmsav7_dregion) {

            return 0;

        }

        return ((cpu->env.pmsav7.dracr[region] & 0xffff) << 16) |

            (cpu->env.pmsav7.drsr[region] & 0xffff);

    }

    case 0xdc0: 

        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {

            goto bad_offset;

        }

        return cpu->env.pmsav8.mair0[attrs.secure];

    case 0xdc4: 

        if (!arm_feature(&cpu->env, ARM_FEATURE_V8)) {

            goto bad_offset;

        }

        return cpu->env.pmsav8.mair1[attrs.secure];

    default:

    bad_offset:

        qemu_log_mask(LOG_GUEST_ERROR, "NVIC: Bad read offset 0x%x\n", offset);

        return 0;

    }

}