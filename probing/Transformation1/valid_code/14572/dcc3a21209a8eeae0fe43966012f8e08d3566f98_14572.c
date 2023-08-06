static void aarch64_tr_translate_insn(DisasContextBase *dcbase, CPUState *cpu)

{

    DisasContext *dc = container_of(dcbase, DisasContext, base);

    CPUARMState *env = cpu->env_ptr;



    if (dc->ss_active && !dc->pstate_ss) {

        

        assert(dc->base.num_insns == 1);

        gen_exception(EXCP_UDEF, syn_swstep(dc->ss_same_el, 0, 0),

                      default_exception_el(dc));

        dc->base.is_jmp = DISAS_NORETURN;

    } else {

        disas_a64_insn(env, dc);

    }



    if (dc->base.is_jmp == DISAS_NEXT) {

        if (dc->ss_active || dc->pc >= dc->next_page_start) {

            dc->base.is_jmp = DISAS_TOO_MANY;

        }

    }



    dc->base.pc_next = dc->pc;

    translator_loop_temp_check(&dc->base);

}