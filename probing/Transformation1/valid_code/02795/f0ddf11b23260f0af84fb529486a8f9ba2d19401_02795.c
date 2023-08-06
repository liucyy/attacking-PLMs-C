DISAS_INSN(cas2l)

{

    uint16_t ext1, ext2;

    TCGv addr1, addr2, regs;



    



    ext1 = read_im16(env, s);



    if (ext1 & 0x8000) {

        

        addr1 = AREG(ext1, 12);

    } else {

        

        addr1 = DREG(ext1, 12);

    }



    ext2 = read_im16(env, s);

    if (ext2 & 0x8000) {

        

        addr2 = AREG(ext2, 12);

    } else {

        

        addr2 = DREG(ext2, 12);

    }



    



    regs = tcg_const_i32(REG(ext2, 6) |

                         (REG(ext1, 6) << 3) |

                         (REG(ext2, 0) << 6) |

                         (REG(ext1, 0) << 9));

    gen_helper_cas2l(cpu_env, regs, addr1, addr2);

    tcg_temp_free(regs);



    

    s->cc_op = CC_OP_CMPL;

    s->cc_op_synced = 1;

}