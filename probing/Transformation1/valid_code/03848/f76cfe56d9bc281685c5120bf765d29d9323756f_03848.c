static void cpu_unlink_tb(CPUState *env)

{

#if defined(CONFIG_USE_NPTL)

    

#else

    TranslationBlock *tb;

    static spinlock_t interrupt_lock = SPIN_LOCK_UNLOCKED;



    tb = env->current_tb;

    

    if (tb && !testandset(&interrupt_lock)) {

        env->current_tb = NULL;

        tb_reset_jump_recursive(tb);

        resetlock(&interrupt_lock);

    }

#endif

}