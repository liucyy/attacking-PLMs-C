static void handle_sys(DisasContext *s, uint32_t insn, bool isread,

                       unsigned int op0, unsigned int op1, unsigned int op2,

                       unsigned int crn, unsigned int crm, unsigned int rt)

{

    const ARMCPRegInfo *ri;

    TCGv_i64 tcg_rt;



    ri = get_arm_cp_reginfo(s->cp_regs,

                            ENCODE_AA64_CP_REG(CP_REG_ARM64_SYSREG_CP,

                                               crn, crm, op0, op1, op2));



    if (!ri) {

        

        qemu_log_mask(LOG_UNIMP, "%s access to unsupported AArch64 "

                      "system register op0:%d op1:%d crn:%d crm:%d op2:%d\n",

                      isread ? "read" : "write", op0, op1, crn, crm, op2);

        unallocated_encoding(s);

        return;

    }



    

    if (!cp_access_ok(s->current_el, ri, isread)) {

        unallocated_encoding(s);

        return;

    }



    if (ri->accessfn) {

        

        TCGv_ptr tmpptr;

        TCGv_i32 tcg_syn;

        uint32_t syndrome;



        gen_a64_set_pc_im(s->pc - 4);

        tmpptr = tcg_const_ptr(ri);

        syndrome = syn_aa64_sysregtrap(op0, op1, op2, crn, crm, rt, isread);

        tcg_syn = tcg_const_i32(syndrome);

        gen_helper_access_check_cp_reg(cpu_env, tmpptr, tcg_syn);

        tcg_temp_free_ptr(tmpptr);

        tcg_temp_free_i32(tcg_syn);

    }



    

    switch (ri->type & ~(ARM_CP_FLAG_MASK & ~ARM_CP_SPECIAL)) {

    case ARM_CP_NOP:

        return;

    case ARM_CP_NZCV:

        tcg_rt = cpu_reg(s, rt);

        if (isread) {

            gen_get_nzcv(tcg_rt);

        } else {

            gen_set_nzcv(tcg_rt);

        }

        return;

    case ARM_CP_CURRENTEL:

        

        tcg_rt = cpu_reg(s, rt);

        tcg_gen_movi_i64(tcg_rt, s->current_el << 2);

        return;

    case ARM_CP_DC_ZVA:

        

        tcg_rt = cpu_reg(s, rt);

        gen_helper_dc_zva(cpu_env, tcg_rt);

        return;

    default:

        break;

    }



    if (use_icount && (ri->type & ARM_CP_IO)) {

        gen_io_start();

    }



    tcg_rt = cpu_reg(s, rt);



    if (isread) {

        if (ri->type & ARM_CP_CONST) {

            tcg_gen_movi_i64(tcg_rt, ri->resetvalue);

        } else if (ri->readfn) {

            TCGv_ptr tmpptr;

            tmpptr = tcg_const_ptr(ri);

            gen_helper_get_cp_reg64(tcg_rt, cpu_env, tmpptr);

            tcg_temp_free_ptr(tmpptr);

        } else {

            tcg_gen_ld_i64(tcg_rt, cpu_env, ri->fieldoffset);

        }

    } else {

        if (ri->type & ARM_CP_CONST) {

            

            return;

        } else if (ri->writefn) {

            TCGv_ptr tmpptr;

            tmpptr = tcg_const_ptr(ri);

            gen_helper_set_cp_reg64(cpu_env, tmpptr, tcg_rt);

            tcg_temp_free_ptr(tmpptr);

        } else {

            tcg_gen_st_i64(tcg_rt, cpu_env, ri->fieldoffset);

        }

    }



    if (use_icount && (ri->type & ARM_CP_IO)) {

        

        gen_io_end();

        s->is_jmp = DISAS_UPDATE;

    } else if (!isread && !(ri->type & ARM_CP_SUPPRESS_TB_END)) {

        

        s->is_jmp = DISAS_UPDATE;

    }

}