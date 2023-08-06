void tlb_fill(target_ulong addr, int is_write, int is_user, void *retaddr)

{

    TranslationBlock *tb;

    int ret;

    unsigned long pc;

    CPUX86State *saved_env;



    

    saved_env = env;

    env = cpu_single_env;



    ret = cpu_x86_handle_mmu_fault(env, addr, is_write, is_user, 1);

    if (ret) {

        if (retaddr) {

            

            pc = (unsigned long)retaddr;

            tb = tb_find_pc(pc);

            if (tb) {

                

                cpu_restore_state(tb, env, pc, NULL);

            }

        }

        if (retaddr)

            raise_exception_err(EXCP0E_PAGE, env->error_code);

        else

            raise_exception_err_norestore(EXCP0E_PAGE, env->error_code);

    }

    env = saved_env;

}