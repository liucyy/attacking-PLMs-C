static void gen_addq(DisasContext *s, TCGv_i64 val, int rlow, int rhigh)

{

    TCGv_i64 tmp;

    TCGv tmpl;

    TCGv tmph;



    

    tmpl = load_reg(s, rlow);

    tmph = load_reg(s, rhigh);

    tmp = tcg_temp_new_i64();

    tcg_gen_concat_i32_i64(tmp, tmpl, tmph);

    dead_tmp(tmpl);

    dead_tmp(tmph);

    tcg_gen_add_i64(val, val, tmp);

    tcg_temp_free_i64(tmp);

}