static void arm_tr_tb_stop(DisasContextBase *dcbase, CPUState *cpu)

{

    DisasContext *dc = container_of(dcbase, DisasContext, base);



    if (dc->base.tb->cflags & CF_LAST_IO && dc->condjmp) {

        

        cpu_abort(cpu, "IO on conditional branch instruction");

    }



    

    gen_set_condexec(dc);

    if (dc->base.is_jmp == DISAS_BX_EXCRET) {

        

        gen_bx_excret_final_code(dc);

    } else if (unlikely(is_singlestepping(dc))) {

        

        switch (dc->base.is_jmp) {

        case DISAS_SWI:

            gen_ss_advance(dc);

            gen_exception(EXCP_SWI, syn_aa32_svc(dc->svc_imm, dc->thumb),

                          default_exception_el(dc));

            break;

        case DISAS_HVC:

            gen_ss_advance(dc);

            gen_exception(EXCP_HVC, syn_aa32_hvc(dc->svc_imm), 2);

            break;

        case DISAS_SMC:

            gen_ss_advance(dc);

            gen_exception(EXCP_SMC, syn_aa32_smc(), 3);

            break;

        case DISAS_NEXT:

        case DISAS_TOO_MANY:

        case DISAS_UPDATE:

            gen_set_pc_im(dc, dc->pc);

            

        default:

            

            gen_singlestep_exception(dc);

            break;

        case DISAS_NORETURN:

            break;

        }

    } else {

        

        switch(dc->base.is_jmp) {

        case DISAS_NEXT:

        case DISAS_TOO_MANY:

            gen_goto_tb(dc, 1, dc->pc);

            break;

        case DISAS_JUMP:

            gen_goto_ptr();

            break;

        case DISAS_UPDATE:

            gen_set_pc_im(dc, dc->pc);

            

        default:

            

            tcg_gen_exit_tb(0);

            break;

        case DISAS_NORETURN:

            

            break;

        case DISAS_WFI:

            gen_helper_wfi(cpu_env);

            

            tcg_gen_exit_tb(0);

            break;

        case DISAS_WFE:

            gen_helper_wfe(cpu_env);

            break;

        case DISAS_YIELD:

            gen_helper_yield(cpu_env);

            break;

        case DISAS_SWI:

            gen_exception(EXCP_SWI, syn_aa32_svc(dc->svc_imm, dc->thumb),

                          default_exception_el(dc));

            break;

        case DISAS_HVC:

            gen_exception(EXCP_HVC, syn_aa32_hvc(dc->svc_imm), 2);

            break;

        case DISAS_SMC:

            gen_exception(EXCP_SMC, syn_aa32_smc(), 3);

            break;

        }

    }



    if (dc->condjmp) {

        

        gen_set_label(dc->condlabel);

        gen_set_condexec(dc);

        if (unlikely(is_singlestepping(dc))) {

            gen_set_pc_im(dc, dc->pc);

            gen_singlestep_exception(dc);

        } else {

            gen_goto_tb(dc, 1, dc->pc);

        }

    }



    

    dc->base.pc_next = dc->pc;

}