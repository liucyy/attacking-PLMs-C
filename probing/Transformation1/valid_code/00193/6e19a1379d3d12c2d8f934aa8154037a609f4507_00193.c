void tlb_fill(CPUState *env1, target_ulong addr, int is_write, int mmu_idx,

              void *retaddr)

{

    TranslationBlock *tb;

    CPUState *saved_env;

    unsigned long pc;

    int ret;



    saved_env = env;


    ret = cpu_arm_handle_mmu_fault(env, addr, is_write, mmu_idx);

    if (unlikely(ret)) {

        if (retaddr) {

            

            pc = (unsigned long)retaddr;

            tb = tb_find_pc(pc);

            if (tb) {

                

                cpu_restore_state(tb, env, pc);

            }

        }

        raise_exception(env->exception_index);

    }

    env = saved_env;

}