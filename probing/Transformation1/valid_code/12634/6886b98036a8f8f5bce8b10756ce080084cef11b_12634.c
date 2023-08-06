void cpu_resume_from_signal(CPUState *cpu, void *puc)

{

    



    cpu->exception_index = -1;

    siglongjmp(cpu->jmp_env, 1);

}