static void apic_reset_common(DeviceState *d)

{

    APICCommonState *s = DO_UPCAST(APICCommonState, busdev.qdev, d);

    APICCommonClass *info = APIC_COMMON_GET_CLASS(s);

    bool bsp;



    bsp = cpu_is_bsp(s->cpu_env);

    s->apicbase = 0xfee00000 |

        (bsp ? MSR_IA32_APICBASE_BSP : 0) | MSR_IA32_APICBASE_ENABLE;



    s->vapic_paddr = 0;

    info->vapic_base_update(s);



    apic_init_reset(d);



    if (bsp) {

        

        s->lvt[APIC_LVT_LINT0] = 0x700;

    }

}