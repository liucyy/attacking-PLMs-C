static TCGv gen_ea(CPUM68KState *env, DisasContext *s, uint16_t insn,

                   int opsize, TCGv val, TCGv *addrp, ea_what what)

{

    TCGv reg;

    TCGv result;

    uint32_t offset;



    switch ((insn >> 3) & 7) {

    case 0: 

        reg = DREG(insn, 0);

        if (what == EA_STORE) {

            gen_partset_reg(opsize, reg, val);

            return store_dummy;

        } else {

            return gen_extend(reg, opsize, what == EA_LOADS);

        }

    case 1: 

        reg = AREG(insn, 0);

        if (what == EA_STORE) {

            tcg_gen_mov_i32(reg, val);

            return store_dummy;

        } else {

            return gen_extend(reg, opsize, what == EA_LOADS);

        }

    case 2: 

        reg = AREG(insn, 0);

        return gen_ldst(s, opsize, reg, val, what);

    case 3: 

        reg = AREG(insn, 0);

        result = gen_ldst(s, opsize, reg, val, what);

        

        if (what == EA_STORE || !addrp)

            tcg_gen_addi_i32(reg, reg, opsize_bytes(opsize));

        return result;

    case 4: 

        {

            TCGv tmp;

            if (addrp && what == EA_STORE) {

                tmp = *addrp;

            } else {

                tmp = gen_lea(env, s, insn, opsize);

                if (IS_NULL_QREG(tmp))

                    return tmp;

                if (addrp)

                    *addrp = tmp;

            }

            result = gen_ldst(s, opsize, tmp, val, what);

            

            if (what == EA_STORE || !addrp) {

                reg = AREG(insn, 0);

                tcg_gen_mov_i32(reg, tmp);

            }

        }

        return result;

    case 5: 

    case 6: 

        return gen_ea_once(env, s, insn, opsize, val, addrp, what);

    case 7: 

        switch (insn & 7) {

        case 0: 

        case 1: 

        case 2: 

        case 3: 

            return gen_ea_once(env, s, insn, opsize, val, addrp, what);

        case 4: 

            

            switch (opsize) {

            case OS_BYTE:

                if (what == EA_LOADS) {

                    offset = cpu_ldsb_code(env, s->pc + 1);

                } else {

                    offset = cpu_ldub_code(env, s->pc + 1);

                }

                s->pc += 2;

                break;

            case OS_WORD:

                if (what == EA_LOADS) {

                    offset = cpu_ldsw_code(env, s->pc);

                } else {

                    offset = cpu_lduw_code(env, s->pc);

                }

                s->pc += 2;

                break;

            case OS_LONG:

                offset = read_im32(env, s);

                break;

            default:

                qemu_assert(0, "Bad immediate operand");

            }

            return tcg_const_i32(offset);

        default:

            return NULL_QREG;

        }

    }

    

    return NULL_QREG;

}