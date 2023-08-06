static void cchip_write(void *opaque, hwaddr addr,

                        uint64_t val, unsigned size)

{

    TyphoonState *s = opaque;

    uint64_t oldval, newval;



    switch (addr) {

    case 0x0000:

        

        

        break;



    case 0x0040:

        

        

        break;



    case 0x0080:

        

        newval = oldval = s->cchip.misc;

        newval &= ~(val & 0x10000ff0);     

        if (val & 0x100000) {

            newval &= ~0xff0000ull;        

        } else {

            newval |= val & 0x00f00000;    

            if ((newval & 0xf0000) == 0) {

                newval |= val & 0xf0000;   

            }

        }

        newval |= (val & 0xf000) >> 4;     



        newval &= ~0xf0000000000ull;       

        newval |= val & 0xf0000000000ull;

        s->cchip.misc = newval;



        

        if ((newval ^ oldval) & 0xff0) {

            int i;

            for (i = 0; i < 4; ++i) {

                AlphaCPU *cpu = s->cchip.cpu[i];

                if (cpu != NULL) {

                    CPUState *cs = CPU(cpu);

                    

                    if (newval & (1 << (i + 8))) {

                        cpu_interrupt(cs, CPU_INTERRUPT_SMP);

                    } else {

                        cpu_reset_interrupt(cs, CPU_INTERRUPT_SMP);

                    }



                    

                    if ((newval & (1 << (i + 4))) == 0) {

                        cpu_reset_interrupt(cs, CPU_INTERRUPT_TIMER);

                    }

                }

            }

        }

        break;



    case 0x00c0:

        

        break;



    case 0x0100: 

    case 0x0140: 

    case 0x0180: 

    case 0x01c0: 

        

        

        break;



    case 0x0200: 

        

        s->cchip.dim[0] = val;

        cpu_irq_change(s->cchip.cpu[0], val & s->cchip.drir);

        break;

    case 0x0240: 

        

        s->cchip.dim[0] = val;

        cpu_irq_change(s->cchip.cpu[1], val & s->cchip.drir);

        break;



    case 0x0280: 

    case 0x02c0: 

    case 0x0300: 

        break;



    case 0x0340:

        

        break;



    case 0x0380: 

        s->cchip.iic[0] = val & 0xffffff;

        break;

    case 0x03c0: 

        s->cchip.iic[1] = val & 0xffffff;

        break;



    case 0x0400: 

    case 0x0440: 

    case 0x0480: 

    case 0x04c0: 

        

        break;



    case 0x0580:

        

        

        break;

    case 0x05c0:

        

        break;



    case 0x0600:

        

        s->cchip.dim[2] = val;

        cpu_irq_change(s->cchip.cpu[2], val & s->cchip.drir);

        break;

    case 0x0640:

        

        s->cchip.dim[3] = val;

        cpu_irq_change(s->cchip.cpu[3], val & s->cchip.drir);

        break;



    case 0x0680: 

    case 0x06c0: 

        break;



    case 0x0700: 

        s->cchip.iic[2] = val & 0xffffff;

        break;

    case 0x0740: 

        s->cchip.iic[3] = val & 0xffffff;

        break;



    case 0x0780:

        

        break;

    

    case 0x0c00: 

    case 0x0c40: 

    case 0x0c80: 

    case 0x0cc0: 

        break;



    default:

        cpu_unassigned_access(current_cpu, addr, true, false, 0, size);

        return;

    }

}