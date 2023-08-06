static void cpu_exec_step(CPUState *cpu)

{

    CPUClass *cc = CPU_GET_CLASS(cpu);

    TranslationBlock *tb;

    target_ulong cs_base, pc;

    uint32_t flags;

    uint32_t cflags = 1 | CF_IGNORE_ICOUNT;



    if (sigsetjmp(cpu->jmp_env, 0) == 0) {

        tb = tb_lookup__cpu_state(cpu, &pc, &cs_base, &flags,

                                  cflags & CF_HASH_MASK);

        if (tb == NULL) {

            mmap_lock();

            tb_lock();

            tb = tb_gen_code(cpu, pc, cs_base, flags, cflags);

            tb_unlock();

            mmap_unlock();

        }



        cc->cpu_exec_enter(cpu);

        

        trace_exec_tb(tb, pc);

        cpu_tb_exec(cpu, tb);

        cc->cpu_exec_exit(cpu);

    } else {

        

#ifndef CONFIG_SOFTMMU

        tcg_debug_assert(!have_mmap_lock());

#endif

        tb_lock_reset();

    }

}