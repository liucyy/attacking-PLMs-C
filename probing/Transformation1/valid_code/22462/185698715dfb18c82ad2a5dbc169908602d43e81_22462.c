uint64_t helper_efdctuidz (uint64_t val)

{

    CPU_DoubleU u;



    u.ll = val;

    

    if (unlikely(float64_is_nan(u.d)))

        return 0;



    return float64_to_uint64_round_to_zero(u.d, &env->vec_status);

}