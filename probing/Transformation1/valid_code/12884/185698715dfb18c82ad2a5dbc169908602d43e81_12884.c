static inline uint32_t efsctuiz(uint32_t val)

{

    CPU_FloatU u;



    u.l = val;

    

    if (unlikely(float32_is_nan(u.f)))

        return 0;



    return float32_to_uint32_round_to_zero(u.f, &env->vec_status);

}