static inline void gen_405_mulladd_insn(DisasContext *ctx, int opc2, int opc3,

                                        int ra, int rb, int rt, int Rc)

{

    TCGv t0, t1;



    t0 = tcg_temp_local_new();

    t1 = tcg_temp_local_new();



    switch (opc3 & 0x0D) {

    case 0x05:

        

        

        

        

        

        tcg_gen_ext16s_tl(t0, cpu_gpr[ra]);

        tcg_gen_sari_tl(t1, cpu_gpr[rb], 16);

        tcg_gen_ext16s_tl(t1, t1);

        break;

    case 0x04:

        

        

        

        tcg_gen_ext16u_tl(t0, cpu_gpr[ra]);

        tcg_gen_shri_tl(t1, cpu_gpr[rb], 16);

        tcg_gen_ext16u_tl(t1, t1);

        break;

    case 0x01:

        

        

        

        

        

        tcg_gen_sari_tl(t0, cpu_gpr[ra], 16);

        tcg_gen_ext16s_tl(t0, t0);

        tcg_gen_sari_tl(t1, cpu_gpr[rb], 16);

        tcg_gen_ext16s_tl(t1, t1);

        break;

    case 0x00:

        

        

        

        tcg_gen_shri_tl(t0, cpu_gpr[ra], 16);

        tcg_gen_ext16u_tl(t0, t0);

        tcg_gen_shri_tl(t1, cpu_gpr[rb], 16);

        tcg_gen_ext16u_tl(t1, t1);

        break;

    case 0x0D:

        

        

        

        

        

        tcg_gen_ext16s_tl(t0, cpu_gpr[ra]);

        tcg_gen_ext16s_tl(t1, cpu_gpr[rb]);

        break;

    case 0x0C:

        

        

        

        tcg_gen_ext16u_tl(t0, cpu_gpr[ra]);

        tcg_gen_ext16u_tl(t1, cpu_gpr[rb]);

        break;

    }

    if (opc2 & 0x04) {

        

        tcg_gen_mul_tl(t1, t0, t1);

        if (opc2 & 0x02) {

            

            tcg_gen_sub_tl(t0, cpu_gpr[rt], t1);

        } else {

            

            tcg_gen_add_tl(t0, cpu_gpr[rt], t1);

        }



        if (opc3 & 0x12) {

            

            int l1 = gen_new_label();



            if (opc3 & 0x10) {

                

                tcg_gen_movi_tl(cpu_ov, 0);

            }

            if (opc3 & 0x01) {

                

                tcg_gen_xor_tl(t1, cpu_gpr[rt], t1);

                tcg_gen_brcondi_tl(TCG_COND_GE, t1, 0, l1);

                tcg_gen_xor_tl(t1, cpu_gpr[rt], t0);

                tcg_gen_brcondi_tl(TCG_COND_LT, t1, 0, l1);

                if (opc3 & 0x02) {

                    

                    tcg_gen_sari_tl(t0, cpu_gpr[rt], 31);

                    tcg_gen_xori_tl(t0, t0, 0x7fffffff);

                }

            } else {

                

                tcg_gen_brcond_tl(TCG_COND_GEU, t0, t1, l1);

                if (opc3 & 0x02) {

                    

                    tcg_gen_movi_tl(t0, UINT32_MAX);

                }

            }

            if (opc3 & 0x10) {

                

                tcg_gen_movi_tl(cpu_ov, 1);

                tcg_gen_movi_tl(cpu_so, 1);

            }

            gen_set_label(l1);

            tcg_gen_mov_tl(cpu_gpr[rt], t0);

        }

    } else {

        tcg_gen_mul_tl(cpu_gpr[rt], t0, t1);

    }

    tcg_temp_free(t0);

    tcg_temp_free(t1);

    if (unlikely(Rc) != 0) {

        

        gen_set_Rc0(ctx, cpu_gpr[rt]);

    }

}