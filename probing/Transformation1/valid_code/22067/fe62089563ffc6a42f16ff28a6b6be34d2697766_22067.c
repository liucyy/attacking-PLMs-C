static inline void gen_goto_tb(DisasContext *s, int tb_num, target_ulong eip)

{

    target_ulong pc = s->cs_base + eip;



    if (use_goto_tb(s, pc))  {

        

        tcg_gen_goto_tb(tb_num);

        gen_jmp_im(eip);

        tcg_gen_exit_tb((uintptr_t)s->tb + tb_num);

    } else {

        

        gen_jmp_im(eip);

        gen_eob(s);

    }

}