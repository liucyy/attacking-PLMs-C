static uint32_t nvic_readl(nvic_state *s, uint32_t offset)

{

    ARMCPU *cpu;

    uint32_t val;

    int irq;



    switch (offset) {

    case 4: 

        return (s->num_irq / 32) - 1;

    case 0x10: 

        val = s->systick.control;

        s->systick.control &= ~SYSTICK_COUNTFLAG;

        return val;

    case 0x14: 

        return s->systick.reload;

    case 0x18: 

        {

            int64_t t;

            if ((s->systick.control & SYSTICK_ENABLE) == 0)

                return 0;

            t = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);

            if (t >= s->systick.tick)

                return 0;

            val = ((s->systick.tick - (t + 1)) / systick_scale(s)) + 1;

            

            if (val > s->systick.reload)

                val = 0;

            return val;

        }

    case 0x1c: 

        return 10000;

    case 0xd00: 

        cpu = ARM_CPU(current_cpu);

        return cpu->env.cp15.c0_cpuid;

    case 0xd04: 

        

        val = s->gic.running_irq[0];

        if (val == 1023) {

            val = 0;

        } else if (val >= 32) {

            val -= 16;

        }

        

        if (s->gic.running_irq[0] == 1023

                || s->gic.last_active[s->gic.running_irq[0]][0] == 1023) {

            val |= (1 << 11);

        }

        

        if (s->gic.current_pending[0] != 1023)

            val |= (s->gic.current_pending[0] << 12);

        

        for (irq = 32; irq < s->num_irq; irq++) {

            if (s->gic.irq_state[irq].pending) {

                val |= (1 << 22);

                break;

            }

        }

        

        if (s->gic.irq_state[ARMV7M_EXCP_SYSTICK].pending)

            val |= (1 << 26);

        

        if (s->gic.irq_state[ARMV7M_EXCP_PENDSV].pending)

            val |= (1 << 28);

        

        if (s->gic.irq_state[ARMV7M_EXCP_NMI].pending)

            val |= (1 << 31);

        return val;

    case 0xd08: 

        cpu = ARM_CPU(current_cpu);

        return cpu->env.v7m.vecbase;

    case 0xd0c: 

        return 0xfa05000;

    case 0xd10: 

        

        return 0;

    case 0xd14: 

        

        return 0;

    case 0xd24: 

        val = 0;

        if (s->gic.irq_state[ARMV7M_EXCP_MEM].active) val |= (1 << 0);

        if (s->gic.irq_state[ARMV7M_EXCP_BUS].active) val |= (1 << 1);

        if (s->gic.irq_state[ARMV7M_EXCP_USAGE].active) val |= (1 << 3);

        if (s->gic.irq_state[ARMV7M_EXCP_SVC].active) val |= (1 << 7);

        if (s->gic.irq_state[ARMV7M_EXCP_DEBUG].active) val |= (1 << 8);

        if (s->gic.irq_state[ARMV7M_EXCP_PENDSV].active) val |= (1 << 10);

        if (s->gic.irq_state[ARMV7M_EXCP_SYSTICK].active) val |= (1 << 11);

        if (s->gic.irq_state[ARMV7M_EXCP_USAGE].pending) val |= (1 << 12);

        if (s->gic.irq_state[ARMV7M_EXCP_MEM].pending) val |= (1 << 13);

        if (s->gic.irq_state[ARMV7M_EXCP_BUS].pending) val |= (1 << 14);

        if (s->gic.irq_state[ARMV7M_EXCP_SVC].pending) val |= (1 << 15);

        if (s->gic.irq_state[ARMV7M_EXCP_MEM].enabled) val |= (1 << 16);

        if (s->gic.irq_state[ARMV7M_EXCP_BUS].enabled) val |= (1 << 17);

        if (s->gic.irq_state[ARMV7M_EXCP_USAGE].enabled) val |= (1 << 18);

        return val;

    case 0xd28: 

        

        qemu_log_mask(LOG_UNIMP, "Configurable Fault Status unimplemented\n");

        return 0;

    case 0xd2c: 

    case 0xd30: 

    case 0xd34: 

    case 0xd38: 

    case 0xd3c: 

        

        qemu_log_mask(LOG_UNIMP, "Fault status registers unimplemented\n");

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

    

    default:

        qemu_log_mask(LOG_GUEST_ERROR, "NVIC: Bad read offset 0x%x\n", offset);

        return 0;

    }

}