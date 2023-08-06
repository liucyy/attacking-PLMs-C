void helper_mwait(CPUX86State *env, int next_eip_addend)

{

    CPUState *cs;

    X86CPU *cpu;



    if ((uint32_t)env->regs[R_ECX] != 0) {

        raise_exception(env, EXCP0D_GPF);

    }

    cpu_svm_check_intercept_param(env, SVM_EXIT_MWAIT, 0);

    env->eip += next_eip_addend;



    cpu = x86_env_get_cpu(env);

    cs = CPU(cpu);

    

    if (cs->cpu_index != 0 || CPU_NEXT(cs) != NULL) {

        

    } else {

        do_hlt(cpu);

    }

}