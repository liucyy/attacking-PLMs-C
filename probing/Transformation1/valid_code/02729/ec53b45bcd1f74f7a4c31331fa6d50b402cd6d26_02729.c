CPUArchState *cpu_copy(CPUArchState *env)

{

    CPUState *cpu = ENV_GET_CPU(env);

    CPUArchState *new_env = cpu_init(cpu_model);

    CPUState *new_cpu = ENV_GET_CPU(new_env);

#if defined(TARGET_HAS_ICE)

    CPUBreakpoint *bp;

    CPUWatchpoint *wp;

#endif



    

    cpu_reset(new_cpu);



    memcpy(new_env, env, sizeof(CPUArchState));



    

    QTAILQ_INIT(&cpu->breakpoints);

    QTAILQ_INIT(&cpu->watchpoints);

#if defined(TARGET_HAS_ICE)

    QTAILQ_FOREACH(bp, &cpu->breakpoints, entry) {

        cpu_breakpoint_insert(new_cpu, bp->pc, bp->flags, NULL);

    }

    QTAILQ_FOREACH(wp, &cpu->watchpoints, entry) {

        cpu_watchpoint_insert(new_cpu, wp->vaddr, wp->len, wp->flags, NULL);

    }

#endif



    return new_env;

}