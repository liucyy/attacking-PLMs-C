static void rtas_stop_self(PowerPCCPU *cpu, sPAPRMachineState *spapr,
                           uint32_t token, uint32_t nargs,
                           target_ulong args,
                           uint32_t nret, target_ulong rets)
{
    CPUState *cs = CPU(cpu);
    CPUPPCState *env = &cpu->env;
    PowerPCCPUClass *pcc = POWERPC_CPU_GET_CLASS(cpu);
    cs->halted = 1;
    qemu_cpu_kick(cs);
    
    env->msr = 0;
}