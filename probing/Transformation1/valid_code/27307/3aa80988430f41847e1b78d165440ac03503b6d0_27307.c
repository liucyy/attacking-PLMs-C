static void dec_load(DisasContext *dc)

{

    TCGv t, *addr;

    unsigned int size;



    size = 1 << (dc->opcode & 3);



    LOG_DIS("l %x %d\n", dc->opcode, size);

    t_sync_flags(dc);

    addr = compute_ldst_addr(dc, &t);



    

    sync_jmpstate(dc);



    

    if ((dc->env->pvr.regs[2] & PVR2_UNALIGNED_EXC_MASK) && size > 1) {

        gen_helper_memalign(*addr, tcg_const_tl(dc->rd),

                            tcg_const_tl(0), tcg_const_tl(size));

    }



    if (dc->rd) {

        gen_load(dc, cpu_R[dc->rd], *addr, size);

    } else {

        gen_load(dc, env_imm, *addr, size);

    }



    if (addr == &t)

        tcg_temp_free(t);

}