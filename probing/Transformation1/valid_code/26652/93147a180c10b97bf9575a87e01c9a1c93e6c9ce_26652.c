target_ulong HELPER(mfspr)(CPUOpenRISCState *env,

                           target_ulong rd, target_ulong ra, uint32_t offset)

{

#ifndef CONFIG_USER_ONLY

    int spr = (ra | offset);

    int idx;



    OpenRISCCPU *cpu = openrisc_env_get_cpu(env);



    switch (spr) {

    case TO_SPR(0, 0): 

        return env->vr & SPR_VR;



    case TO_SPR(0, 1): 

        return env->upr;    



    case TO_SPR(0, 2): 

        return env->cpucfgr;



    case TO_SPR(0, 3): 

        return env->dmmucfgr;    



    case TO_SPR(0, 4): 

        return env->immucfgr;



    case TO_SPR(0, 16): 

        return env->npc;



    case TO_SPR(0, 17): 

        return env->sr;



    case TO_SPR(0, 18): 

        return env->ppc;



    case TO_SPR(0, 32): 

        return env->epcr;



    case TO_SPR(0, 48): 

        return env->eear;



    case TO_SPR(0, 64): 

        return env->esr;



    case TO_SPR(1, 512) ... TO_SPR(1, 639): 

        idx = spr - TO_SPR(1, 512);

        return env->tlb->dtlb[0][idx].mr;



    case TO_SPR(1, 640) ... TO_SPR(1, 767): 

        idx = spr - TO_SPR(1, 640);

        return env->tlb->dtlb[0][idx].tr;



    case TO_SPR(1, 768) ... TO_SPR(1, 895):   

    case TO_SPR(1, 896) ... TO_SPR(1, 1023):  

    case TO_SPR(1, 1024) ... TO_SPR(1, 1151): 

    case TO_SPR(1, 1152) ... TO_SPR(1, 1279): 

    case TO_SPR(1, 1280) ... TO_SPR(1, 1407): 

    case TO_SPR(1, 1408) ... TO_SPR(1, 1535): 

        break;



    case TO_SPR(2, 512) ... TO_SPR(2, 639): 

        idx = spr - TO_SPR(2, 512);

        return env->tlb->itlb[0][idx].mr;



    case TO_SPR(2, 640) ... TO_SPR(2, 767): 

        idx = spr - TO_SPR(2, 640);

        return env->tlb->itlb[0][idx].tr;



    case TO_SPR(2, 768) ... TO_SPR(2, 895):   

    case TO_SPR(2, 896) ... TO_SPR(2, 1023):  

    case TO_SPR(2, 1024) ... TO_SPR(2, 1151): 

    case TO_SPR(2, 1152) ... TO_SPR(2, 1279): 

    case TO_SPR(2, 1280) ... TO_SPR(2, 1407): 

    case TO_SPR(2, 1408) ... TO_SPR(2, 1535): 

        break;



    case TO_SPR(9, 0):  

        return env->picmr;



    case TO_SPR(9, 2):  

        return env->picsr;



    case TO_SPR(10, 0): 

        return env->ttmr;



    case TO_SPR(10, 1): 

        cpu_openrisc_count_update(cpu);

        return env->ttcr;



    default:

        break;

    }

#endif







    

    return rd;

}