uint64_t helper_fctiw(CPUPPCState *env, uint64_t arg)

{

    CPU_DoubleU farg;



    farg.ll = arg;



    if (unlikely(float64_is_signaling_nan(farg.d))) {

        

        farg.ll = fload_invalid_op_excp(env, POWERPC_EXCP_FP_VXSNAN |

                                        POWERPC_EXCP_FP_VXCVI);

    } else if (unlikely(float64_is_quiet_nan(farg.d) ||

                        float64_is_infinity(farg.d))) {

        

        farg.ll = fload_invalid_op_excp(env, POWERPC_EXCP_FP_VXCVI);

    } else {

        farg.ll = float64_to_int32(farg.d, &env->fp_status);

        

        farg.ll |= 0xFFF80000ULL << 32;

    }

    return farg.ll;

}