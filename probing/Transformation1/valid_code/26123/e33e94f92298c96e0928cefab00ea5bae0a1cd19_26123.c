static always_inline uint64_t float_zero_divide_excp (uint64_t arg1, uint64_t arg2)

{

    env->fpscr |= 1 << FPSCR_ZX;

    env->fpscr &= ~((1 << FPSCR_FR) | (1 << FPSCR_FI));

    

    env->fpscr |= 1 << FPSCR_FX;

    if (fpscr_ze != 0) {

        

        env->fpscr |= 1 << FPSCR_FEX;

        if (msr_fe0 != 0 || msr_fe1 != 0) {

            helper_raise_exception_err(POWERPC_EXCP_PROGRAM,

                                       POWERPC_EXCP_FP | POWERPC_EXCP_FP_ZX);

        }

    } else {

        

        arg1 = ((arg1 ^ arg2) & 0x8000000000000000ULL);

        arg1 |= 0x7FFULL << 52;

    }

    return arg1;

}