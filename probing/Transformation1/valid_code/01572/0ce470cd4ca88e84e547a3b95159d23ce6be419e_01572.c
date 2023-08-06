int kvmppc_fixup_cpu(PowerPCCPU *cpu)

{

    CPUState *cs = CPU(cpu);

    int smt;



    

    smt = kvmppc_smt_threads();

    cs->cpu_index = (cs->cpu_index / smp_threads) * smt

        + (cs->cpu_index % smp_threads);



    return 0;

}