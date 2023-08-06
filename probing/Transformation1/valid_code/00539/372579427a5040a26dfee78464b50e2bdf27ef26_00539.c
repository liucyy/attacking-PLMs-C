int cpu_exec(CPUState *cpu)

{

    CPUClass *cc = CPU_GET_CLASS(cpu);

    int ret;

    SyncClocks sc;



    

    current_cpu = cpu;



    if (cpu_handle_halt(cpu)) {

        return EXCP_HALTED;

    }



    rcu_read_lock();



    cc->cpu_exec_enter(cpu);



    

    init_delay_params(&sc, cpu);



    

    if (sigsetjmp(cpu->jmp_env, 0) != 0) {

#if defined(__clang__) || !QEMU_GNUC_PREREQ(4, 6)

        

        cpu = current_cpu;

        cc = CPU_GET_CLASS(cpu);

#else 

        

        g_assert(cpu == current_cpu);

        g_assert(cc == CPU_GET_CLASS(cpu));

#endif 

        cpu->can_do_io = 1;

        tb_lock_reset();

        if (qemu_mutex_iothread_locked()) {

            qemu_mutex_unlock_iothread();

        }

    }



    

    while (!cpu_handle_exception(cpu, &ret)) {

        TranslationBlock *last_tb = NULL;

        int tb_exit = 0;



        while (!cpu_handle_interrupt(cpu, &last_tb)) {

            TranslationBlock *tb = tb_find(cpu, last_tb, tb_exit);

            cpu_loop_exec_tb(cpu, tb, &last_tb, &tb_exit, &sc);

            

            align_clocks(&sc, cpu);

        }

    }



    cc->cpu_exec_exit(cpu);

    rcu_read_unlock();



    

    current_cpu = NULL;



    return ret;

}