static bool tcg_out_opc_jmp(TCGContext *s, MIPSInsn opc, void *target)

{

    uintptr_t dest = (uintptr_t)target;

    uintptr_t from = (uintptr_t)s->code_ptr + 4;

    int32_t inst;



    

    if ((from ^ dest) & -(1 << 28)) {

        return false;

    }

    assert((dest & 3) == 0);



    inst = opc;

    inst |= (dest >> 2) & 0x3ffffff;

    tcg_out32(s, inst);

    return true;

}