static inline TranslationBlock *tb_find(CPUState *cpu,

                                        TranslationBlock *last_tb,

                                        int tb_exit)

{

    TranslationBlock *tb;

    target_ulong cs_base, pc;

    uint32_t flags;

    bool acquired_tb_lock = false;

    uint32_t cf_mask = curr_cflags();



    tb = tb_lookup__cpu_state(cpu, &pc, &cs_base, &flags, cf_mask);

    if (tb == NULL) {

        

        mmap_lock();

        tb_lock();

        acquired_tb_lock = true;



        

        tb = tb_htable_lookup(cpu, pc, cs_base, flags, cf_mask);

        if (likely(tb == NULL)) {

            

            tb = tb_gen_code(cpu, pc, cs_base, flags, cf_mask);

        }



        mmap_unlock();

        

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

        if (!(tb->cflags & CF_INVALID)) {

            tb_add_jump(last_tb, tb_exit, tb);

        }

    }

    if (acquired_tb_lock) {

        tb_unlock();

    }

    return tb;

}