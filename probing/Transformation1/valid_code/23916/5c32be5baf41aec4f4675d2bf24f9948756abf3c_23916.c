static void tcg_out_qemu_ld(TCGContext *s, TCGReg data, TCGReg addr,

                            TCGMemOpIdx oi, bool is_64)

{

    TCGMemOp memop = get_memop(oi);

#ifdef CONFIG_SOFTMMU

    unsigned memi = get_mmuidx(oi);

    TCGReg addrz, param;

    tcg_insn_unit *func;

    tcg_insn_unit *label_ptr;



    addrz = tcg_out_tlb_load(s, addr, memi, memop,

                             offsetof(CPUTLBEntry, addr_read));



    



    

    label_ptr = s->code_ptr;

    tcg_out_bpcc0(s, COND_E, BPCC_A | BPCC_PT

                  | (TARGET_LONG_BITS == 64 ? BPCC_XCC : BPCC_ICC), 0);

    

    tcg_out_ldst_rr(s, data, addrz, TCG_REG_O1,

                    qemu_ld_opc[memop & (MO_BSWAP | MO_SSIZE)]);



    



    param = TCG_REG_O1;

    if (!SPARC64 && TARGET_LONG_BITS == 64) {

        

        param++;

    }

    tcg_out_mov(s, TCG_TYPE_REG, param++, addr);



    

    if ((memop & MO_SSIZE) == MO_SL) {

        func = qemu_ld_trampoline[memop & (MO_BSWAP | MO_SIZE)];

    } else {

        func = qemu_ld_trampoline[memop & (MO_BSWAP | MO_SSIZE)];

    }

    tcg_debug_assert(func != NULL);

    tcg_out_call_nodelay(s, func);

    

    tcg_out_movi(s, TCG_TYPE_I32, param, oi);



    

    if (SPARC64) {

        

        if (is_64 && (memop & MO_SSIZE) == MO_SL) {

            tcg_out_arithi(s, data, TCG_REG_O0, 0, SHIFT_SRA);

        } else {

            tcg_out_mov(s, TCG_TYPE_REG, data, TCG_REG_O0);

        }

    } else {

        if ((memop & MO_SIZE) == MO_64) {

            tcg_out_arithi(s, TCG_REG_O0, TCG_REG_O0, 32, SHIFT_SLLX);

            tcg_out_arithi(s, TCG_REG_O1, TCG_REG_O1, 0, SHIFT_SRL);

            tcg_out_arith(s, data, TCG_REG_O0, TCG_REG_O1, ARITH_OR);

        } else if (is_64) {

            

            tcg_out_arithi(s, data, TCG_REG_O1, 0,

                           memop & MO_SIGN ? SHIFT_SRA : SHIFT_SRL);

        } else {

            tcg_out_mov(s, TCG_TYPE_I32, data, TCG_REG_O1);

        }

    }



    *label_ptr |= INSN_OFF19(tcg_ptr_byte_diff(s->code_ptr, label_ptr));

#else

    if (SPARC64 && TARGET_LONG_BITS == 32) {

        tcg_out_arithi(s, TCG_REG_T1, addr, 0, SHIFT_SRL);

        addr = TCG_REG_T1;

    }

    tcg_out_ldst_rr(s, data, addr,

                    (guest_base ? TCG_GUEST_BASE_REG : TCG_REG_G0),

                    qemu_ld_opc[memop & (MO_BSWAP | MO_SSIZE)]);

#endif 

}