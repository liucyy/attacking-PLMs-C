static void machine_cpu_reset(MicroBlazeCPU *cpu)

{

    CPUMBState *env = &cpu->env;



    env->pvr.regs[10] = 0x0e000000; 

    

    env->pvr.regs[0] |= (0x14 << 8);

    env->pvr.regs[4] = 0xc56b8000;

    env->pvr.regs[5] = 0xc56be000;

}