void cpu_exit(CPUState *cpu)

{

    cpu->exit_request = 1;

    

    smp_wmb();

    cpu->tcg_exit_req = 1;

}