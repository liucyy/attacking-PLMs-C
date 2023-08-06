static void gen_movl_seg_T0(DisasContext *s, int seg_reg)

{

    if (s->pe && !s->vm86) {

        tcg_gen_trunc_tl_i32(cpu_tmp2_i32, cpu_T0);

        gen_helper_load_seg(cpu_env, tcg_const_i32(seg_reg), cpu_tmp2_i32);

        

        if (seg_reg == R_SS || (s->code32 && seg_reg < R_FS))

            s->is_jmp = DISAS_TB_JUMP;

    } else {

        gen_op_movl_seg_T0_vm(seg_reg);

        if (seg_reg == R_SS)

            s->is_jmp = DISAS_TB_JUMP;

    }

}