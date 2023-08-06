static void arm_tr_translate_insn(DisasContextBase *dcbase, CPUState *cpu)

{

    DisasContext *dc = container_of(dcbase, DisasContext, base);

    CPUARMState *env = cpu->env_ptr;



#ifdef CONFIG_USER_ONLY

    

    if (dc->pc >= 0xffff0000) {

        

        gen_exception_internal(EXCP_KERNEL_TRAP);

        dc->base.is_jmp = DISAS_NORETURN;

        return;

    }

#endif



    if (dc->ss_active && !dc->pstate_ss) {

        

        assert(dc->base.num_insns == 1);

        gen_exception(EXCP_UDEF, syn_swstep(dc->ss_same_el, 0, 0),

                      default_exception_el(dc));

        dc->base.is_jmp = DISAS_NORETURN;

        return;

    }



    if (dc->thumb) {

        disas_thumb_insn(env, dc);

        if (dc->condexec_mask) {

            dc->condexec_cond = (dc->condexec_cond & 0xe)

                | ((dc->condexec_mask >> 4) & 1);

            dc->condexec_mask = (dc->condexec_mask << 1) & 0x1f;

            if (dc->condexec_mask == 0) {

                dc->condexec_cond = 0;

            }

        }

    } else {

        unsigned int insn = arm_ldl_code(env, dc->pc, dc->sctlr_b);

        dc->pc += 4;

        disas_arm_insn(dc, insn);

    }



    if (dc->condjmp && !dc->base.is_jmp) {

        gen_set_label(dc->condlabel);

        dc->condjmp = 0;

    }



    if (dc->base.is_jmp == DISAS_NEXT) {

        



        if (dc->pc >= dc->next_page_start ||

            (dc->pc >= dc->next_page_start - 3 &&

             insn_crosses_page(env, dc))) {

            

            dc->base.is_jmp = DISAS_TOO_MANY;

        }

    }



    dc->base.pc_next = dc->pc;

    translator_loop_temp_check(&dc->base);

}