static inline TranslationBlock *tb_find_fast(CPUState *cpu,

                                             TranslationBlock *last_tb,

                                             int tb_exit)

{

    CPUArchState *env = (CPUArchState *)cpu->env_ptr;

    TranslationBlock *tb;

    target_ulong cs_base, pc;

    uint32_t flags;



    

    cpu_get_tb_cpu_state(env, &pc, &cs_base, &flags);

    tb_lock();

    tb = atomic_rcu_read(&cpu->tb_jmp_cache[tb_jmp_cache_hash_func(pc)]);

    if (unlikely(!tb || tb->pc != pc || tb->cs_base != cs_base ||

                 tb->flags != flags)) {

        tb = tb_find_slow(cpu, pc, cs_base, flags);

    }

#ifndef CONFIG_USER_ONLY

    

    if (tb->page_addr[1] != -1) {

        last_tb = NULL;

    }

#endif

    

    if (last_tb && !qemu_loglevel_mask(CPU_LOG_TB_NOCHAIN)) {

        

        if (cpu->tb_flushed) {

            cpu->tb_flushed = false;

        } else {

            tb_add_jump(last_tb, tb_exit, tb);

        }

    }

    tb_unlock();

    return tb;

}