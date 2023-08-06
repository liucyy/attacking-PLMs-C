static bool linked_bp_matches(ARMCPU *cpu, int lbn)

{

    CPUARMState *env = &cpu->env;

    uint64_t bcr = env->cp15.dbgbcr[lbn];

    int brps = extract32(cpu->dbgdidr, 24, 4);

    int ctx_cmps = extract32(cpu->dbgdidr, 20, 4);

    int bt;

    uint32_t contextidr;



    

    if (lbn > brps || lbn < (brps - ctx_cmps)) {

        return false;

    }



    bcr = env->cp15.dbgbcr[lbn];



    if (extract64(bcr, 0, 1) == 0) {

        

        return false;

    }



    bt = extract64(bcr, 20, 4);



    

    contextidr = extract64(env->cp15.contextidr_el1, 0, 32);



    switch (bt) {

    case 3: 

        if (arm_current_el(env) > 1) {

            

            return false;

        }

        return (contextidr == extract64(env->cp15.dbgbvr[lbn], 0, 32));

    case 5: 

    case 9: 

    case 11: 

    default:

        

        return false;

    }



    return false;

}