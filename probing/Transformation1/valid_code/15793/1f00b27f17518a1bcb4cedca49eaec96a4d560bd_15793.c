static TCGReg tcg_out_tlb_read(TCGContext *s, TCGMemOp opc,

                               TCGReg addrlo, TCGReg addrhi,

                               int mem_index, bool is_read)

{

    int cmp_off

        = (is_read

           ? offsetof(CPUArchState, tlb_table[mem_index][0].addr_read)

           : offsetof(CPUArchState, tlb_table[mem_index][0].addr_write));

    int add_off = offsetof(CPUArchState, tlb_table[mem_index][0].addend);

    TCGReg base = TCG_AREG0;

    TCGMemOp s_bits = opc & MO_SIZE;



    

    if (TCG_TARGET_REG_BITS == 64) {

        if (TARGET_LONG_BITS == 32) {

            

            tcg_out_ext32u(s, TCG_REG_R4, addrlo);

            addrlo = TCG_REG_R4;

        } else {

            tcg_out_rld(s, RLDICL, TCG_REG_R3, addrlo,

                        64 - TARGET_PAGE_BITS, 64 - CPU_TLB_BITS);

        }

    }



    

    if (add_off >= 0x8000) {

        

        QEMU_BUILD_BUG_ON(offsetof(CPUArchState,

                                   tlb_table[NB_MMU_MODES - 1][1])

                          > 0x7ff0 + 0x7fff);

        tcg_out32(s, ADDI | TAI(TCG_REG_TMP1, base, 0x7ff0));

        base = TCG_REG_TMP1;

        cmp_off -= 0x7ff0;

        add_off -= 0x7ff0;

    }



    

    if (TCG_TARGET_REG_BITS == 32 || TARGET_LONG_BITS == 32) {

        tcg_out_rlw(s, RLWINM, TCG_REG_R3, addrlo,

                    32 - (TARGET_PAGE_BITS - CPU_TLB_ENTRY_BITS),

                    32 - (CPU_TLB_BITS + CPU_TLB_ENTRY_BITS),

                    31 - CPU_TLB_ENTRY_BITS);

    } else {

        tcg_out_shli64(s, TCG_REG_R3, TCG_REG_R3, CPU_TLB_ENTRY_BITS);

    }



    tcg_out32(s, ADD | TAB(TCG_REG_R3, TCG_REG_R3, base));



    

    if (TCG_TARGET_REG_BITS < TARGET_LONG_BITS) {

        tcg_out_ld(s, TCG_TYPE_I32, TCG_REG_R4, TCG_REG_R3, cmp_off);

        tcg_out_ld(s, TCG_TYPE_I32, TCG_REG_TMP1, TCG_REG_R3, cmp_off + 4);

    } else {

        tcg_out_ld(s, TCG_TYPE_TL, TCG_REG_TMP1, TCG_REG_R3, cmp_off);

    }



    

    tcg_out_ld(s, TCG_TYPE_PTR, TCG_REG_R3, TCG_REG_R3, add_off);



    

    if (TCG_TARGET_REG_BITS == 32 || TARGET_LONG_BITS == 32) {

        

        tcg_out_rlw(s, RLWINM, TCG_REG_R0, addrlo, 0,

                    (32 - s_bits) & 31, 31 - TARGET_PAGE_BITS);

    } else if (s_bits) {

        

        if ((opc & MO_AMASK) == MO_ALIGN) {

            

            tcg_out_rld(s, RLDICL, TCG_REG_R0, addrlo,

                        64 - TARGET_PAGE_BITS, TARGET_PAGE_BITS - s_bits);

            tcg_out_rld(s, RLDICL, TCG_REG_R0, TCG_REG_R0, TARGET_PAGE_BITS, 0);

       } else {

           

            tcg_out32(s, ADDI | TAI(TCG_REG_R0, addrlo, (1 << s_bits) - 1));

            tcg_out_rld(s, RLDICR, TCG_REG_R0, TCG_REG_R0,

                        0, 63 - TARGET_PAGE_BITS);

        }

    } else {

        

        tcg_out_rld(s, RLDICR, TCG_REG_R0, addrlo, 0, 63 - TARGET_PAGE_BITS);

    }



    if (TCG_TARGET_REG_BITS < TARGET_LONG_BITS) {

        tcg_out_cmp(s, TCG_COND_EQ, TCG_REG_R0, TCG_REG_TMP1,

                    0, 7, TCG_TYPE_I32);

        tcg_out_cmp(s, TCG_COND_EQ, addrhi, TCG_REG_R4, 0, 6, TCG_TYPE_I32);

        tcg_out32(s, CRAND | BT(7, CR_EQ) | BA(6, CR_EQ) | BB(7, CR_EQ));

    } else {

        tcg_out_cmp(s, TCG_COND_EQ, TCG_REG_R0, TCG_REG_TMP1,

                    0, 7, TCG_TYPE_TL);

    }



    return addrlo;

}