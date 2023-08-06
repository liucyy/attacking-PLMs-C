void QEMU_NORETURN runtime_exception(CPUS390XState *env, int excp,

                                     uintptr_t retaddr)

{

    CPUState *cs = CPU(s390_env_get_cpu(env));

    int t;



    cs->exception_index = EXCP_PGM;

    env->int_pgm_code = excp;



    

    cpu_restore_state(cs, retaddr);



    

    t = cpu_ldub_code(env, env->psw.addr);

    env->int_pgm_ilen = t = get_ilen(t);

    env->psw.addr += t;



    cpu_loop_exit(cs);

}