static void tcg_out_tlb_read(TCGContext *s, TCGReg addrlo, TCGReg addrhi,

                             int s_bits, int tlb_offset)

{

    TCGReg base = TCG_AREG0;



    

#  if CPU_TLB_BITS > 8

#   error

#  endif

    tcg_out_dat_reg(s, COND_AL, ARITH_MOV, TCG_REG_TMP,

                    0, addrlo, SHIFT_IMM_LSR(TARGET_PAGE_BITS));



    

    assert((tlb_offset & ~0xffff) == 0);

    if (tlb_offset > 0xff) {

        tcg_out_dat_imm(s, COND_AL, ARITH_ADD, TCG_REG_R2, base,

                        (24 << 7) | (tlb_offset >> 8));

        tlb_offset &= 0xff;

        base = TCG_REG_R2;

    }



    tcg_out_dat_imm(s, COND_AL, ARITH_AND,

                    TCG_REG_R0, TCG_REG_TMP, CPU_TLB_SIZE - 1);

    tcg_out_dat_reg(s, COND_AL, ARITH_ADD, TCG_REG_R2, base,

                    TCG_REG_R0, SHIFT_IMM_LSL(CPU_TLB_ENTRY_BITS));



    

    if (use_armv6_instructions && TARGET_LONG_BITS == 64) {

        tcg_out_memop_8(s, COND_AL, INSN_LDRD_IMM, TCG_REG_R0,

                        TCG_REG_R2, tlb_offset, 1, 1);

    } else {

        tcg_out_memop_12(s, COND_AL, INSN_LDR_IMM, TCG_REG_R0,

                         TCG_REG_R2, tlb_offset, 1, 1);

        if (TARGET_LONG_BITS == 64) {

            tcg_out_memop_12(s, COND_AL, INSN_LDR_IMM, TCG_REG_R1,

                             TCG_REG_R2, 4, 1, 0);

        }

    }



    

    if (s_bits) {

        tcg_out_dat_imm(s, COND_AL, ARITH_TST,

                        0, addrlo, (1 << s_bits) - 1);

    }



    tcg_out_dat_reg(s, (s_bits ? COND_EQ : COND_AL), ARITH_CMP, 0,

                    TCG_REG_R0, TCG_REG_TMP, SHIFT_IMM_LSL(TARGET_PAGE_BITS));



    if (TARGET_LONG_BITS == 64) {

        tcg_out_dat_reg(s, COND_EQ, ARITH_CMP, 0,

                        TCG_REG_R1, addrhi, SHIFT_IMM_LSL(0));

    }

}