static ExitStatus op_ex(DisasContext *s, DisasOps *o)

{

    



    TCGv_i64 tmp;



    update_psw_addr(s);

    update_cc_op(s);



    tmp = tcg_const_i64(s->next_pc);

    gen_helper_ex(cc_op, cpu_env, cc_op, o->in1, o->in2, tmp);

    tcg_temp_free_i64(tmp);



    set_cc_static(s);

    return NO_EXIT;

}