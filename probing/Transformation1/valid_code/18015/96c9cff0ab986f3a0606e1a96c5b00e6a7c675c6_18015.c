static void kvm_get_fallback_smmu_info(PowerPCCPU *cpu,

                                       struct kvm_ppc_smmu_info *info)

{

    CPUPPCState *env = &cpu->env;

    CPUState *cs = CPU(cpu);



    memset(info, 0, sizeof(*info));



    

    if (kvm_check_extension(cs->kvm_state, KVM_CAP_PPC_GET_PVINFO)) {

        

        info->flags = 0;

        info->slb_size = 64;



        

        info->sps[0].page_shift = 12;

        info->sps[0].slb_enc = 0;

        info->sps[0].enc[0].page_shift = 12;

        info->sps[0].enc[0].pte_enc = 0;



        

        info->sps[1].page_shift = 24;

        info->sps[1].slb_enc = SLB_VSID_L;

        info->sps[1].enc[0].page_shift = 24;

        info->sps[1].enc[0].pte_enc = 0;

    } else {

        int i = 0;



        

        info->flags = KVM_PPC_PAGE_SIZES_REAL;



        if (env->mmu_model & POWERPC_MMU_1TSEG) {

            info->flags |= KVM_PPC_1T_SEGMENTS;

        }



        if (env->mmu_model == POWERPC_MMU_2_06 ||

            env->mmu_model == POWERPC_MMU_2_07) {

            info->slb_size = 32;

        } else {

            info->slb_size = 64;

        }



        

        info->sps[i].page_shift = 12;

        info->sps[i].slb_enc = 0;

        info->sps[i].enc[0].page_shift = 12;

        info->sps[i].enc[0].pte_enc = 0;

        i++;



        

        if (env->mmu_model == POWERPC_MMU_2_06 ||

            env->mmu_model == POWERPC_MMU_2_07) {

            info->sps[i].page_shift = 16;

            info->sps[i].slb_enc = 0x110;

            info->sps[i].enc[0].page_shift = 16;

            info->sps[i].enc[0].pte_enc = 1;

            i++;

        }



        

        info->sps[i].page_shift = 24;

        info->sps[i].slb_enc = SLB_VSID_L;

        info->sps[i].enc[0].page_shift = 24;

        info->sps[i].enc[0].pte_enc = 0;

    }

}