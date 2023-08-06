static void host_x86_cpu_initfn(Object *obj)

{

    X86CPU *cpu = X86_CPU(obj);

    CPUX86State *env = &cpu->env;

    KVMState *s = kvm_state;



    

    cpu->host_features = true;



    

    if (kvm_enabled()) {

        env->cpuid_level = kvm_arch_get_supported_cpuid(s, 0x0, 0, R_EAX);

        env->cpuid_xlevel = kvm_arch_get_supported_cpuid(s, 0x80000000, 0, R_EAX);

        env->cpuid_xlevel2 = kvm_arch_get_supported_cpuid(s, 0xC0000000, 0, R_EAX);

    }



    object_property_set_bool(OBJECT(cpu), true, "pmu", &error_abort);

}