static void gen_nop_hint(DisasContext *s, int val)

{

    switch (val) {

    case 1: 

        if (!parallel_cpus) {

            gen_set_pc_im(s, s->pc);

            s->base.is_jmp = DISAS_YIELD;

        }

        break;

    case 3: 

        gen_set_pc_im(s, s->pc);

        s->base.is_jmp = DISAS_WFI;

        break;

    case 2: 

        if (!parallel_cpus) {

            gen_set_pc_im(s, s->pc);

            s->base.is_jmp = DISAS_WFE;

        }

        break;

    case 4: 

    case 5: 

        

    default: 

        break;

    }

}