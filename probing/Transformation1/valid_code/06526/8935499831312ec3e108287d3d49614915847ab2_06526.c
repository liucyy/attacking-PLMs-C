static void get_cpuid_vendor(CPUX86State *env, uint32_t *ebx,

                             uint32_t *ecx, uint32_t *edx)

{

    *ebx = env->cpuid_vendor1;

    *edx = env->cpuid_vendor2;

    *ecx = env->cpuid_vendor3;



    

    if (kvm_enabled() && env->cpuid_vendor_override) {

        host_cpuid(0, 0, NULL, ebx, ecx, edx);

    }

}