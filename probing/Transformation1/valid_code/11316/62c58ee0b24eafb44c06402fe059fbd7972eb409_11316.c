static bool get_phys_addr_pmsav8(CPUARMState *env, uint32_t address,

                                 MMUAccessType access_type, ARMMMUIdx mmu_idx,

                                 hwaddr *phys_ptr, int *prot, uint32_t *fsr)

{

    ARMCPU *cpu = arm_env_get_cpu(env);

    bool is_user = regime_is_user(env, mmu_idx);

    int n;

    int matchregion = -1;

    bool hit = false;



    *phys_ptr = address;

    *prot = 0;



    

    if (regime_translation_disabled(env, mmu_idx)) { 

        hit = true;

    } else if (m_is_ppb_region(env, address)) {

        hit = true;

    } else if (pmsav7_use_background_region(cpu, mmu_idx, is_user)) {

        hit = true;

    } else {

        for (n = (int)cpu->pmsav7_dregion - 1; n >= 0; n--) {

            

            

            uint32_t base = env->pmsav8.rbar[n] & ~0x1f;

            uint32_t limit = env->pmsav8.rlar[n] | 0x1f;



            if (!(env->pmsav8.rlar[n] & 0x1)) {

                

                continue;

            }



            if (address < base || address > limit) {

                continue;

            }



            if (hit) {

                

                *fsr = 0x00d; 

                return true;

            }



            matchregion = n;

            hit = true;



            if (base & ~TARGET_PAGE_MASK) {

                qemu_log_mask(LOG_UNIMP,

                              "MPU_RBAR[%d]: No support for MPU region base"

                              "address of 0x%" PRIx32 ". Minimum alignment is "

                              "%d\n",

                              n, base, TARGET_PAGE_BITS);

                continue;

            }

            if ((limit + 1) & ~TARGET_PAGE_MASK) {

                qemu_log_mask(LOG_UNIMP,

                              "MPU_RBAR[%d]: No support for MPU region limit"

                              "address of 0x%" PRIx32 ". Minimum alignment is "

                              "%d\n",

                              n, limit, TARGET_PAGE_BITS);

                continue;

            }

        }

    }



    if (!hit) {

        

        *fsr = 0;

        return true;

    }



    if (matchregion == -1) {

        

        get_phys_addr_pmsav7_default(env, mmu_idx, address, prot);

    } else {

        uint32_t ap = extract32(env->pmsav8.rbar[matchregion], 1, 2);

        uint32_t xn = extract32(env->pmsav8.rbar[matchregion], 0, 1);



        if (m_is_system_region(env, address)) {

            

            xn = 1;

        }



        *prot = simple_ap_to_rw_prot(env, mmu_idx, ap);

        if (*prot && !xn) {

            *prot |= PAGE_EXEC;

        }

        

    }



    *fsr = 0x00d; 

    return !(*prot & (1 << access_type));

}