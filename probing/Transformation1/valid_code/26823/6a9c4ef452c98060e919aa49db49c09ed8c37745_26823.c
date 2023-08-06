void cpu_ppc_set_papr(PowerPCCPU *cpu)

{

    CPUPPCState *env = &cpu->env;




    

    env->msr_mask &= ~((1ull << MSR_EP) | MSR_HVB);



    

    env->spr[SPR_AMOR] = amor->default_value = 0xffffffffffffffffull;



    

    if (kvm_enabled()) {

        kvmppc_set_papr(cpu);

    }

}