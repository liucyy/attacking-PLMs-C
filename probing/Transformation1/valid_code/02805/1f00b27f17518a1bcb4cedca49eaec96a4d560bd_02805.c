static void tcg_out_tlb_read(TCGContext *s, TCGReg addr_reg, TCGMemOp opc,

                             tcg_insn_unit **label_ptr, int mem_index,

                             bool is_read)

{

    int tlb_offset = is_read ?

        offsetof(CPUArchState, tlb_table[mem_index][0].addr_read)

        : offsetof(CPUArchState, tlb_table[mem_index][0].addr_write);

    int s_mask = (1 << (opc & MO_SIZE)) - 1;

    TCGReg base = TCG_AREG0, x3;

    uint64_t tlb_mask;



    

    if ((opc & MO_AMASK) == MO_ALIGN || s_mask == 0) {

        tlb_mask = TARGET_PAGE_MASK | s_mask;

        x3 = addr_reg;

    } else {

        tcg_out_insn(s, 3401, ADDI, TARGET_LONG_BITS == 64,

                     TCG_REG_X3, addr_reg, s_mask);

        tlb_mask = TARGET_PAGE_MASK;

        x3 = TCG_REG_X3;

    }



    

    tcg_out_ubfm(s, TARGET_LONG_BITS == 64, TCG_REG_X0, addr_reg,

                 TARGET_PAGE_BITS, TARGET_PAGE_BITS + CPU_TLB_BITS);



    

    tcg_out_logicali(s, I3404_ANDI, TARGET_LONG_BITS == 64,

                     TCG_REG_X3, x3, tlb_mask);



    

    if (tlb_offset & 0xfff000) {

        tcg_out_insn(s, 3401, ADDI, TCG_TYPE_I64, TCG_REG_X2, base,

                     tlb_offset & 0xfff000);

        base = TCG_REG_X2;

    }



    

    tcg_out_insn(s, 3502S, ADD_LSL, TCG_TYPE_I64, TCG_REG_X2, base,

                 TCG_REG_X0, CPU_TLB_ENTRY_BITS);



    

    tcg_out_ldst(s, TARGET_LONG_BITS == 32 ? I3312_LDRW : I3312_LDRX,

                 TCG_REG_X0, TCG_REG_X2, tlb_offset & 0xfff);



    

    tcg_out_ldst(s, I3312_LDRX, TCG_REG_X1, TCG_REG_X2,

                 (tlb_offset & 0xfff) + (offsetof(CPUTLBEntry, addend)) -

                 (is_read ? offsetof(CPUTLBEntry, addr_read)

                  : offsetof(CPUTLBEntry, addr_write)));



    

    tcg_out_cmp(s, (TARGET_LONG_BITS == 64), TCG_REG_X0, TCG_REG_X3, 0);



    

    *label_ptr = s->code_ptr;

    tcg_out_goto_cond_noaddr(s, TCG_COND_NE);

}