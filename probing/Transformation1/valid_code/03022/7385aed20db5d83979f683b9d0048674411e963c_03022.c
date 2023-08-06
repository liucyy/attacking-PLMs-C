float64 helper_fitod(CPUSPARCState *env, int32_t src)

{

    

    return int32_to_float64(src, &env->fp_status);

}