float64 HELPER(sub_cmp_f64)(CPUState *env, float64 a, float64 b)

{

    

    

    float64 res;

    res = float64_sub(a, b, &env->fp_status);

    if (float64_is_nan(res)) {

        

        if (!float64_is_nan(a)

            && !float64_is_nan(b)) {

            res = float64_zero;

            if (float64_lt_quiet(a, res, &env->fp_status))

                res = float64_chs(res);

        }

    }

    return res;

}