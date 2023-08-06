void cpu_exec_exit(CPUState *cpu)

{

    if (cpu->cpu_index == -1) {

        

        return;

    }



    bitmap_clear(cpu_index_map, cpu->cpu_index, 1);

    cpu->cpu_index = -1;

}