void cpu_exec_step_atomic(CPUState *cpu)

{

    start_exclusive();



    

    parallel_cpus = false;

    cpu_exec_step(cpu);

    parallel_cpus = true;



    end_exclusive();

}