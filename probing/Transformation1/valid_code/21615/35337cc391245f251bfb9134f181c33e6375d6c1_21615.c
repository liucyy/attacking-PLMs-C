static bool get_phys_addr(CPUARMState *env, target_ulong address,

                          MMUAccessType access_type, ARMMMUIdx mmu_idx,

                          hwaddr *phys_ptr, MemTxAttrs *attrs, int *prot,

                          target_ulong *page_size, uint32_t *fsr,

                          ARMMMUFaultInfo *fi)

{

    if (mmu_idx == ARMMMUIdx_S12NSE0 || mmu_idx == ARMMMUIdx_S12NSE1) {

        

        if (arm_feature(env, ARM_FEATURE_EL2)) {

            hwaddr ipa;

            int s2_prot;

            int ret;



            ret = get_phys_addr(env, address, access_type,

                                stage_1_mmu_idx(mmu_idx), &ipa, attrs,

                                prot, page_size, fsr, fi);



            

            if (ret || regime_translation_disabled(env, ARMMMUIdx_S2NS)) {

                *phys_ptr = ipa;

                return ret;

            }



            

            ret = get_phys_addr_lpae(env, ipa, access_type, ARMMMUIdx_S2NS,

                                     phys_ptr, attrs, &s2_prot,

                                     page_size, fsr, fi);

            fi->s2addr = ipa;

            

            *prot &= s2_prot;

            return ret;

        } else {

            

            mmu_idx = stage_1_mmu_idx(mmu_idx);

        }

    }



    

    attrs->secure = regime_is_secure(env, mmu_idx);

    attrs->user = regime_is_user(env, mmu_idx);



    

    if (address < 0x02000000 && mmu_idx != ARMMMUIdx_S2NS

        && !arm_feature(env, ARM_FEATURE_V8)) {

        if (regime_el(env, mmu_idx) == 3) {

            address += env->cp15.fcseidr_s;

        } else {

            address += env->cp15.fcseidr_ns;

        }

    }



    if (arm_feature(env, ARM_FEATURE_PMSA)) {

        bool ret;

        *page_size = TARGET_PAGE_SIZE;



        if (arm_feature(env, ARM_FEATURE_V8)) {

            

            ret = get_phys_addr_pmsav8(env, address, access_type, mmu_idx,

                                       phys_ptr, prot, fsr);

        } else if (arm_feature(env, ARM_FEATURE_V7)) {

            

            ret = get_phys_addr_pmsav7(env, address, access_type, mmu_idx,

                                       phys_ptr, prot, fsr);

        } else {

            

            ret = get_phys_addr_pmsav5(env, address, access_type, mmu_idx,

                                       phys_ptr, prot, fsr);

        }

        qemu_log_mask(CPU_LOG_MMU, "PMSA MPU lookup for %s at 0x%08" PRIx32

                      " mmu_idx %u -> %s (prot %c%c%c)\n",

                      access_type == MMU_DATA_LOAD ? "reading" :

                      (access_type == MMU_DATA_STORE ? "writing" : "execute"),

                      (uint32_t)address, mmu_idx,

                      ret ? "Miss" : "Hit",

                      *prot & PAGE_READ ? 'r' : '-',

                      *prot & PAGE_WRITE ? 'w' : '-',

                      *prot & PAGE_EXEC ? 'x' : '-');



        return ret;

    }



    



    if (regime_translation_disabled(env, mmu_idx)) {

        

        *phys_ptr = address;

        *prot = PAGE_READ | PAGE_WRITE | PAGE_EXEC;

        *page_size = TARGET_PAGE_SIZE;

        return 0;

    }



    if (regime_using_lpae_format(env, mmu_idx)) {

        return get_phys_addr_lpae(env, address, access_type, mmu_idx, phys_ptr,

                                  attrs, prot, page_size, fsr, fi);

    } else if (regime_sctlr(env, mmu_idx) & SCTLR_XP) {

        return get_phys_addr_v6(env, address, access_type, mmu_idx, phys_ptr,

                                attrs, prot, page_size, fsr, fi);

    } else {

        return get_phys_addr_v5(env, address, access_type, mmu_idx, phys_ptr,

                                prot, page_size, fsr, fi);

    }

}