static uint32_t cas_check_pvr(PowerPCCPU *cpu, target_ulong *addr,

                              Error **errp)

{

    bool explicit_match = false; 

    uint32_t max_compat = cpu->max_compat;

    uint32_t best_compat = 0;

    int i;



    

    for (i = 0; i < 512; ++i) {

        uint32_t pvr, pvr_mask;



        pvr_mask = ldl_be_phys(&address_space_memory, *addr);

        pvr = ldl_be_phys(&address_space_memory, *addr + 4);

        *addr += 8;



        if (~pvr_mask & pvr) {

            break; 

        }



        if ((cpu->env.spr[SPR_PVR] & pvr_mask) == (pvr & pvr_mask)) {

            explicit_match = true;

        } else {

            if (ppc_check_compat(cpu, pvr, best_compat, max_compat)) {

                best_compat = pvr;

            }

        }

    }



    if ((best_compat == 0) && (!explicit_match || max_compat)) {

        

        error_setg(errp, "Couldn't negotiate a suitable PVR during CAS");

        return 0;

    }



    

    trace_spapr_cas_pvr(cpu->compat_pvr, explicit_match, best_compat);



    return best_compat;

}