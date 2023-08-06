static void sigp_cpu_reset(void *arg)

{

    CPUState *cpu = arg;

    S390CPUClass *scc = S390_CPU_GET_CLASS(cpu);



    cpu_synchronize_state(cpu);

    scc->cpu_reset(cpu);

    cpu_synchronize_post_reset(cpu);

}