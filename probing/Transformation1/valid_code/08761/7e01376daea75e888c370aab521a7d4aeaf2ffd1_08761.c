void ioinst_handle_sal(S390CPU *cpu, uint64_t reg1)

{

    

    if (SAL_REG1_INVALID(reg1) || reg1 & 0x000000000000ffff) {

        program_interrupt(&cpu->env, PGM_OPERAND, 2);

    }

}