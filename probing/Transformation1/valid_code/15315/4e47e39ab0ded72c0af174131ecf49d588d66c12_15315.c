static void update_sse_status(CPUX86State *env)

{

    int rnd_type;



    

    switch (env->mxcsr & SSE_RC_MASK) {

    default:

    case SSE_RC_NEAR:

        rnd_type = float_round_nearest_even;

        break;

    case SSE_RC_DOWN:

        rnd_type = float_round_down;

        break;

    case SSE_RC_UP:

        rnd_type = float_round_up;

        break;

    case SSE_RC_CHOP:

        rnd_type = float_round_to_zero;

        break;

    }

    set_float_rounding_mode(rnd_type, &env->sse_status);



    

    set_flush_inputs_to_zero((env->mxcsr & SSE_DAZ) ? 1 : 0, &env->sse_status);



    

    set_flush_to_zero((env->mxcsr & SSE_FZ) ? 1 : 0, &env->fp_status);

}