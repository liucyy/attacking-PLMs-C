void HELPER(srst)(CPUS390XState *env, uint32_t r1, uint32_t r2)

{

    uintptr_t ra = GETPC();

    uint64_t end, str;

    uint32_t len;

    uint8_t v, c = env->regs[0];



    

    if (env->regs[0] & 0xffffff00u) {

        cpu_restore_state(ENV_GET_CPU(env), ra);

        program_interrupt(env, PGM_SPECIFICATION, 6);

    }



    str = get_address(env, r2);

    end = get_address(env, r1);



    

    for (len = 0; len < 0x2000; ++len) {

        if (str + len == end) {

            

            env->cc_op = 2;

            return;

        }

        v = cpu_ldub_data_ra(env, str + len, ra);

        if (v == c) {

            

            env->cc_op = 1;

            set_address(env, r1, str + len);

            return;

        }

    }



    

    env->cc_op = 3;

    set_address(env, r2, str + len);

}