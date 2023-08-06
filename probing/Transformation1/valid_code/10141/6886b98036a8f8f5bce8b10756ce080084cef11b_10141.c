void s390x_cpu_debug_excp_handler(CPUState *cs)

{

    S390CPU *cpu = S390_CPU(cs);

    CPUS390XState *env = &cpu->env;

    CPUWatchpoint *wp_hit = cs->watchpoint_hit;



    if (wp_hit && wp_hit->flags & BP_CPU) {

        

        cs->watchpoint_hit = NULL;



        env->per_address = env->psw.addr;

        env->per_perc_atmid |= PER_CODE_EVENT_STORE | get_per_atmid(env);

        

        env->per_perc_atmid |= env->psw.mask & (PSW_MASK_ASC) >> 46;



        

        cpu_watchpoint_remove_all(cs, BP_CPU);

        cpu_resume_from_signal(cs, NULL);

    }

}