void helper_fxtoq(CPUSPARCState *env, int64_t src)

{

    

    QT0 = int64_to_float128(src, &env->fp_status);

}