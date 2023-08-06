static void rtas_stop_self(PowerPCCPU *cpu, sPAPRMachineState *spapr,

                           uint32_t token, uint32_t nargs,

                           target_ulong args,

                           uint32_t nret, target_ulong rets)

{

    CPUState *cs = CPU(cpu);

    CPUPPCState *env = &cpu->env;




    cs->halted = 1;

    qemu_cpu_kick(cs);

    

    env->msr = 0;



    

    env->spr[SPR_LPCR] &= ~pcc->lpcr_pm;

}