uint64_t helper_cvttq_svic(CPUAlphaState *env, uint64_t a)

{

    return inline_cvttq(env, a, float_round_to_zero, 1);

}