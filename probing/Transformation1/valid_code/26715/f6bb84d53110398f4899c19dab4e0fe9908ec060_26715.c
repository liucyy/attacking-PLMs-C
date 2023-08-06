static inline TranslationBlock *tb_find(CPUState *cpu,

                                        TranslationBlock *last_tb,

                                        int tb_exit)

{

    CPUArchState *env = (CPUArchState *)cpu->env_ptr;

    TranslationBlock *tb;

    target_ulong cs_base, pc;

    uint32_t flags;

    bool acquired_tb_lock = false;



    

    cpu_get_tb_cpu_state(env, &pc, &cs_base, &flags);

    tb = atomic_rcu_read(&cpu->tb_jmp_cache[tb_jmp_cache_hash_func(pc)]);

    if (unlikely(!tb || tb->pc != pc || tb->cs_base != cs_base ||

                 tb->flags != flags ||

                 tb->trace_vcpu_dstate != *cpu->trace_dstate)) {

        tb = tb_htable_lookup(cpu, pc, cs_base, flags);

        if (!tb) {



            

            mmap_lock();

            tb_lock();

            acquired_tb_lock = true;



            

            tb = tb_htable_lookup(cpu, pc, cs_base, flags);

            if (!tb) {

                

                tb = tb_gen_code(cpu, pc, cs_base, flags, 0);

            }



            mmap_unlock();

        }



        

        atomic_set(&cpu->tb_jmp_cache[tb_jmp_cache_hash_func(pc)], tb);

    }

#ifndef CONFIG_USER_ONLY

    

    if (tb->page_addr[1] != -1) {

        last_tb = NULL;

    }

#endif

    

    if (last_tb && !qemu_loglevel_mask(CPU_LOG_TB_NOCHAIN)) {

        if (!acquired_tb_lock) {

            tb_lock();

            acquired_tb_lock = true;

        }

        if (!tb->invalid) {

            tb_add_jump(last_tb, tb_exit, tb);

        }

    }

    if (acquired_tb_lock) {

        tb_unlock();

    }

    return tb;

}