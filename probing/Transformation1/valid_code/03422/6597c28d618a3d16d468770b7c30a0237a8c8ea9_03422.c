static void dec_float(DisasContext *dc, uint32_t insn)

{

    uint32_t op0;

    uint32_t ra, rb, rd;

    op0 = extract32(insn, 0, 8);

    ra = extract32(insn, 16, 5);

    rb = extract32(insn, 11, 5);

    rd = extract32(insn, 21, 5);



    switch (op0) {

    case 0x00:    

        LOG_DIS("lf.add.s r%d, r%d, r%d\n", rd, ra, rb);

        gen_helper_float_add_s(cpu_R[rd], cpu_env, cpu_R[ra], cpu_R[rb]);

        break;



    case 0x01:    

        LOG_DIS("lf.sub.s r%d, r%d, r%d\n", rd, ra, rb);

        gen_helper_float_sub_s(cpu_R[rd], cpu_env, cpu_R[ra], cpu_R[rb]);

        break;





    case 0x02:    

        LOG_DIS("lf.mul.s r%d, r%d, r%d\n", rd, ra, rb);

        if (ra != 0 && rb != 0) {

            gen_helper_float_mul_s(cpu_R[rd], cpu_env, cpu_R[ra], cpu_R[rb]);

        } else {

            tcg_gen_ori_tl(fpcsr, fpcsr, FPCSR_ZF);

            tcg_gen_movi_i32(cpu_R[rd], 0x0);

        }

        break;



    case 0x03:    

        LOG_DIS("lf.div.s r%d, r%d, r%d\n", rd, ra, rb);

        gen_helper_float_div_s(cpu_R[rd], cpu_env, cpu_R[ra], cpu_R[rb]);

        break;



    case 0x04:    

        LOG_DIS("lf.itof r%d, r%d\n", rd, ra);

        gen_helper_itofs(cpu_R[rd], cpu_env, cpu_R[ra]);

        break;



    case 0x05:    

        LOG_DIS("lf.ftoi r%d, r%d\n", rd, ra);

        gen_helper_ftois(cpu_R[rd], cpu_env, cpu_R[ra]);

        break;



    case 0x06:    

        LOG_DIS("lf.rem.s r%d, r%d, r%d\n", rd, ra, rb);

        gen_helper_float_rem_s(cpu_R[rd], cpu_env, cpu_R[ra], cpu_R[rb]);

        break;



    case 0x07:    

        LOG_DIS("lf.madd.s r%d, r%d, r%d\n", rd, ra, rb);

        gen_helper_float_madd_s(cpu_R[rd], cpu_env, cpu_R[rd],

                                cpu_R[ra], cpu_R[rb]);

        break;



    case 0x08:    

        LOG_DIS("lf.sfeq.s r%d, r%d\n", ra, rb);

        gen_helper_float_eq_s(cpu_sr_f, cpu_env, cpu_R[ra], cpu_R[rb]);

        break;



    case 0x09:    

        LOG_DIS("lf.sfne.s r%d, r%d\n", ra, rb);

        gen_helper_float_ne_s(cpu_sr_f, cpu_env, cpu_R[ra], cpu_R[rb]);

        break;



    case 0x0a:    

        LOG_DIS("lf.sfgt.s r%d, r%d\n", ra, rb);

        gen_helper_float_gt_s(cpu_sr_f, cpu_env, cpu_R[ra], cpu_R[rb]);

        break;



    case 0x0b:    

        LOG_DIS("lf.sfge.s r%d, r%d\n", ra, rb);

        gen_helper_float_ge_s(cpu_sr_f, cpu_env, cpu_R[ra], cpu_R[rb]);

        break;



    case 0x0c:    

        LOG_DIS("lf.sflt.s r%d, r%d\n", ra, rb);

        gen_helper_float_lt_s(cpu_sr_f, cpu_env, cpu_R[ra], cpu_R[rb]);

        break;



    case 0x0d:    

        LOG_DIS("lf.sfle.s r%d, r%d\n", ra, rb);

        gen_helper_float_le_s(cpu_sr_f, cpu_env, cpu_R[ra], cpu_R[rb]);

        break;









    default:

        gen_illegal_exception(dc);

        break;

    }

}