static void tcg_target_qemu_prologue (TCGContext *s)

{

    int i, frame_size;

#ifndef __APPLE__

    uint64_t addr;

#endif



    frame_size = 0

        + 8                     

        + 8                     

        + 8                     

        + 8                     

        + 8                     

        + 8                     

        + TCG_STATIC_CALL_ARGS_SIZE

        + ARRAY_SIZE (tcg_target_callee_save_regs) * 8

        ;

    frame_size = (frame_size + 15) & ~15;



#ifndef __APPLE__

    

    addr = (uint64_t) s->code_ptr + 24;

    tcg_out32 (s, addr >> 32); tcg_out32 (s, addr); 

    s->code_ptr += 16;          

#endif



    

    tcg_out32 (s, MFSPR | RT (0) | LR);

    tcg_out32 (s, STDU | RS (1) | RA (1) | (-frame_size & 0xffff));

    for (i = 0; i < ARRAY_SIZE (tcg_target_callee_save_regs); ++i)

        tcg_out32 (s, (STD

                       | RS (tcg_target_callee_save_regs[i])

                       | RA (1)

                       | (i * 8 + 48 + TCG_STATIC_CALL_ARGS_SIZE)

                       )

            );

    tcg_out32 (s, STD | RS (0) | RA (1) | (frame_size + 16));



#ifdef CONFIG_USE_GUEST_BASE

    if (GUEST_BASE) {

        tcg_out_movi (s, TCG_TYPE_I64, TCG_GUEST_BASE_REG, GUEST_BASE);

        tcg_regset_set_reg(s->reserved_regs, TCG_GUEST_BASE_REG);

    }

#endif



    tcg_out32 (s, MTSPR | RS (3) | CTR);

    tcg_out32 (s, BCCTR | BO_ALWAYS);



    

    tb_ret_addr = s->code_ptr;



    for (i = 0; i < ARRAY_SIZE (tcg_target_callee_save_regs); ++i)

        tcg_out32 (s, (LD

                       | RT (tcg_target_callee_save_regs[i])

                       | RA (1)

                       | (i * 8 + 48 + TCG_STATIC_CALL_ARGS_SIZE)

                       )

            );

    tcg_out32 (s, LD | RT (0) | RA (1) | (frame_size + 16));

    tcg_out32 (s, MTSPR | RS (0) | LR);

    tcg_out32 (s, ADDI | RT (1) | RA (1) | frame_size);

    tcg_out32 (s, BCLR | BO_ALWAYS);

}