static void apic_reset(void *opaque)

{

    APICState *s = opaque;

    int bsp = cpu_is_bsp(s->cpu_env);



    s->apicbase = 0xfee00000 |

        (bsp ? MSR_IA32_APICBASE_BSP : 0) | MSR_IA32_APICBASE_ENABLE;



    apic_init_ipi(s);



    if (bsp) {

        

        s->lvt[APIC_LVT_LINT0] = 0x700;

    }

}