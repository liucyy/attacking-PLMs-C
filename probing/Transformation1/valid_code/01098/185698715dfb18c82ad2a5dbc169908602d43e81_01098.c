static inline uint32_t efsctsiz(uint32_t val)

{

    CPU_FloatU u;



    u.l = val;

    

    if (unlikely(float32_is_nan(u.f)))

        return 0;



    return float32_to_int32_round_to_zero(u.f, &env->vec_status);

}