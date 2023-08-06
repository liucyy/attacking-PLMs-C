static void arm_post_translate_insn(CPUARMState *env, DisasContext *dc)

{

    if (dc->condjmp && !dc->base.is_jmp) {

        gen_set_label(dc->condlabel);

        dc->condjmp = 0;

    }



    

    if (dc->base.is_jmp == DISAS_NEXT

        && (dc->pc >= dc->next_page_start

            || (dc->pc >= dc->next_page_start - 3

                && insn_crosses_page(env, dc)))) {

        dc->base.is_jmp = DISAS_TOO_MANY;

    }



    dc->base.pc_next = dc->pc;

    translator_loop_temp_check(&dc->base);

}