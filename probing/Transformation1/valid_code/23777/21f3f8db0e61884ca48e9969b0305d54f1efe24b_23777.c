static void spapr_dt_ov5_platform_support(void *fdt, int chosen)

{

    PowerPCCPU *first_ppc_cpu = POWERPC_CPU(first_cpu);



    char val[2 * 4] = {

        23, 0x00, 

        24, 0x00, 

        25, 0x00, 

        26, 0x40, 

    };



    if (kvm_enabled()) {

        if (kvmppc_has_cap_mmu_radix() && kvmppc_has_cap_mmu_hash_v3()) {

            val[3] = 0x80; 

        } else if (kvmppc_has_cap_mmu_radix()) {

            val[3] = 0x40; 

        } else {

            val[3] = 0x00; 

        }

    } else {

        if (first_ppc_cpu->env.mmu_model & POWERPC_MMU_V3) {

            

            val[3] = 0xC0;

        } else {

            

            val[3] = 0x00;

        }

    }

    _FDT(fdt_setprop(fdt, chosen, "ibm,arch-vec-5-platform-support",

                     val, sizeof(val)));

}