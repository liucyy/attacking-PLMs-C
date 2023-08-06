static void i386_tr_translate_insn(DisasContextBase *dcbase, CPUState *cpu)

{

    DisasContext *dc = container_of(dcbase, DisasContext, base);

    target_ulong pc_next = disas_insn(dc, cpu);



    if (dc->tf || (dc->base.tb->flags & HF_INHIBIT_IRQ_MASK)) {

        

        

        dc->base.is_jmp = DISAS_TOO_MANY;

    } else if ((dc->base.tb->cflags & CF_USE_ICOUNT)

               && ((dc->base.pc_next & TARGET_PAGE_MASK)

                   != ((dc->base.pc_next + TARGET_MAX_INSN_SIZE - 1)

                       & TARGET_PAGE_MASK)

                   || (dc->base.pc_next & ~TARGET_PAGE_MASK) == 0)) {

        

        dc->base.is_jmp = DISAS_TOO_MANY;

    } else if ((pc_next - dc->base.pc_first) >= (TARGET_PAGE_SIZE - 32)) {

        dc->base.is_jmp = DISAS_TOO_MANY;

    }



    dc->base.pc_next = pc_next;

}