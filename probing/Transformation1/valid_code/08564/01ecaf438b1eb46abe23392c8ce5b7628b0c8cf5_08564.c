void HELPER(dc_zva)(CPUARMState *env, uint64_t vaddr_in)

{

    



    ARMCPU *cpu = arm_env_get_cpu(env);

    uint64_t blocklen = 4 << cpu->dcz_blocksize;

    uint64_t vaddr = vaddr_in & ~(blocklen - 1);



#ifndef CONFIG_USER_ONLY

    {

        

        int maxidx = DIV_ROUND_UP(blocklen, TARGET_PAGE_SIZE);

        void *hostaddr[maxidx];

        int try, i;

        unsigned mmu_idx = cpu_mmu_index(env, false);

        TCGMemOpIdx oi = make_memop_idx(MO_UB, mmu_idx);



        for (try = 0; try < 2; try++) {



            for (i = 0; i < maxidx; i++) {

                hostaddr[i] = tlb_vaddr_to_host(env,

                                                vaddr + TARGET_PAGE_SIZE * i,

                                                1, mmu_idx);

                if (!hostaddr[i]) {

                    break;

                }

            }

            if (i == maxidx) {

                

                for (i = 0; i < maxidx - 1; i++) {

                    memset(hostaddr[i], 0, TARGET_PAGE_SIZE);

                }

                memset(hostaddr[i], 0, blocklen - (i * TARGET_PAGE_SIZE));

                return;

            }

            

            helper_ret_stb_mmu(env, vaddr_in, 0, oi, GETRA());

            

            for (i = 0; i < maxidx; i++) {

                uint64_t va = vaddr + TARGET_PAGE_SIZE * i;

                if (va != (vaddr_in & TARGET_PAGE_MASK)) {

                    helper_ret_stb_mmu(env, va, 0, oi, GETRA());

                }

            }

        }



        

        for (i = 0; i < blocklen; i++) {

            helper_ret_stb_mmu(env, vaddr + i, 0, oi, GETRA());

        }

    }

#else

    memset(g2h(vaddr), 0, blocklen);

#endif

}