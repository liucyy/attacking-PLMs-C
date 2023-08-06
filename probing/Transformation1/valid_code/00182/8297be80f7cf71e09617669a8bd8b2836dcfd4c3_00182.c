static int ppc_fixup_cpu(PowerPCCPU *cpu)

{

    CPUPPCState *env = &cpu->env;



    

    if ((env->insns_flags & ~PPC_TCG_INSNS)

        || (env->insns_flags2 & ~PPC_TCG_INSNS2)) {

        fprintf(stderr, "Warning: Disabling some instructions which are not "

                "emulated by TCG (0x%" PRIx64 ", 0x%" PRIx64 ")\n",

                env->insns_flags & ~PPC_TCG_INSNS,

                env->insns_flags2 & ~PPC_TCG_INSNS2);

    }

    env->insns_flags &= PPC_TCG_INSNS;

    env->insns_flags2 &= PPC_TCG_INSNS2;

    return 0;

}