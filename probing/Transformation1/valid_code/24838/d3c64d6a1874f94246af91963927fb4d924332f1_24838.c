static void x86_cpu_apic_init(X86CPU *cpu, Error **errp)

{

    static int apic_mapped;

    CPUX86State *env = &cpu->env;

    APICCommonState *apic;

    const char *apic_type = "apic";



    if (kvm_irqchip_in_kernel()) {

        apic_type = "kvm-apic";

    } else if (xen_enabled()) {

        apic_type = "xen-apic";

    }



    env->apic_state = qdev_try_create(NULL, apic_type);

    if (env->apic_state == NULL) {

        error_setg(errp, "APIC device '%s' could not be created", apic_type);

        return;

    }



    object_property_add_child(OBJECT(cpu), "apic",

                              OBJECT(env->apic_state), NULL);

    qdev_prop_set_uint8(env->apic_state, "id", env->cpuid_apic_id);

    

    apic = APIC_COMMON(env->apic_state);

    apic->cpu = cpu;



    if (qdev_init(env->apic_state)) {

        error_setg(errp, "APIC device '%s' could not be initialized",

                   object_get_typename(OBJECT(env->apic_state)));

        return;

    }



    

    if (apic_mapped == 0) {

        

        

        sysbus_mmio_map_overlap(SYS_BUS_DEVICE(env->apic_state), 0,

                                APIC_DEFAULT_ADDRESS, 0x1000);

        apic_mapped = 1;

    }

}