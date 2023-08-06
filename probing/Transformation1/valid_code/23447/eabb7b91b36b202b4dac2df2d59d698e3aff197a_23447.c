static inline uint32_t reloc_pc16_val(tcg_insn_unit *pc, tcg_insn_unit *target)

{

    

    ptrdiff_t disp = target - (pc + 1);

    assert(disp == (int16_t)disp);

    return disp & 0xffff;

}