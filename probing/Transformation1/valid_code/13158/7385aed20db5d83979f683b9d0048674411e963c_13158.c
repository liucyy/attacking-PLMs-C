float32 helper_fitos(CPUSPARCState *env, int32_t src)

{

    

    float32 ret;

    clear_float_exceptions(env);

    ret = int32_to_float32(src, &env->fp_status);

    check_ieee_exceptions(env);

    return ret;

}