IEEE_ARITH2(sqrtt)

IEEE_ARITH2(cvtst)

IEEE_ARITH2(cvtts)



static void gen_cvttq(DisasContext *ctx, int rb, int rc, int fn11)

{

    TCGv vb, vc;



    

    vb = gen_ieee_input(ctx, rb, fn11, 0);

    vc = dest_fpr(ctx, rc);



    

    switch (fn11) {

    case QUAL_RM_C:

        gen_helper_cvttq_c(vc, cpu_env, vb);

        break;

    case QUAL_V | QUAL_RM_C:

    case QUAL_S | QUAL_V | QUAL_RM_C:

    case QUAL_S | QUAL_V | QUAL_I | QUAL_RM_C:

        gen_helper_cvttq_svic(vc, cpu_env, vb);

        break;

    default:

        gen_qual_roundmode(ctx, fn11);

        gen_helper_cvttq(vc, cpu_env, vb);

        break;

    }



    gen_fp_exc_raise(rc, fn11);

}