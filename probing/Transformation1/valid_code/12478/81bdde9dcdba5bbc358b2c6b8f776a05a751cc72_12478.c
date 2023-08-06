uint32_t HELPER(get_cp15)(CPUARMState *env, uint32_t insn)

{

    int op1;

    int op2;

    int crm;



    op1 = (insn >> 21) & 7;

    op2 = (insn >> 5) & 7;

    crm = insn & 0xf;

    switch ((insn >> 16) & 0xf) {

    case 0: 

        switch (op1) {

        case 0:

            switch (crm) {

            case 0:

                switch (op2) {

                case 0: 

                    return env->cp15.c0_cpuid;

                case 1: 

		    return env->cp15.c0_cachetype;

                case 2: 

                    return 0;

                case 3: 

                    return 0; 

                case 5: 

                    

                    if (arm_feature(env, ARM_FEATURE_V7) ||

                        ARM_CPUID(env) == ARM_CPUID_ARM11MPCORE) {

                        int mpidr = env->cpu_index;

                        

                        if (arm_feature(env, ARM_FEATURE_V7MP)) {

                            mpidr |= (1 << 31);

                            

                        }

                        return mpidr;

                    }

                    

                default:

                    goto bad_reg;

                }

            case 3: case 4: case 5: case 6: case 7:

                return 0;

            default:

                goto bad_reg;

            }

        default:

            goto bad_reg;

        }

    case 4: 

        goto bad_reg;

    case 11: 

    case 12: 

        goto bad_reg;

    }

bad_reg:

    

    cpu_abort(env, "Unimplemented cp15 register read (c%d, c%d, {%d, %d})\n",

              (insn >> 16) & 0xf, crm, op1, op2);

    return 0;

}