static target_ulong h_client_architecture_support(PowerPCCPU *cpu,

                                                  sPAPRMachineState *spapr,

                                                  target_ulong opcode,

                                                  target_ulong *args)

{

    target_ulong list = ppc64_phys_to_real(args[0]);

    target_ulong ov_table;

    bool explicit_match = false; 

    uint32_t max_compat = cpu->max_compat;

    uint32_t best_compat = 0;

    int i;

    sPAPROptionVector *ov5_guest, *ov5_cas_old, *ov5_updates;

    bool guest_radix;



    

    for (i = 0; i < 512; ++i) {

        uint32_t pvr, pvr_mask;



        pvr_mask = ldl_be_phys(&address_space_memory, list);

        pvr = ldl_be_phys(&address_space_memory, list + 4);

        list += 8;



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

        

        return H_HARDWARE;

    }



    

    trace_spapr_cas_pvr(cpu->compat_pvr, explicit_match, best_compat);



    

    if (cpu->compat_pvr != best_compat) {

        Error *local_err = NULL;



        ppc_set_compat_all(best_compat, &local_err);

        if (local_err) {

            error_report_err(local_err);

            return H_HARDWARE;

        }

    }



    

    ov_table = list;



    ov5_guest = spapr_ovec_parse_vector(ov_table, 5);

    if (spapr_ovec_test(ov5_guest, OV5_MMU_BOTH)) {

        error_report("guest requested hash and radix MMU, which is invalid.");

        exit(EXIT_FAILURE);

    }

    

    guest_radix = spapr_ovec_test(ov5_guest, OV5_MMU_RADIX_300);

    spapr_ovec_clear(ov5_guest, OV5_MMU_RADIX_300);



    

    ov5_cas_old = spapr_ovec_clone(spapr->ov5_cas);

    

    spapr_ovec_intersect(spapr->ov5_cas, spapr->ov5, ov5_guest);

    spapr_ovec_cleanup(ov5_guest);

    

    ov5_updates = spapr_ovec_new();

    spapr->cas_reboot = spapr_ovec_diff(ov5_updates,

                                        ov5_cas_old, spapr->ov5_cas);

    

    if (guest_radix) {

        if (kvm_enabled() && !kvmppc_has_cap_mmu_radix()) {

            error_report("Guest requested unavailable MMU mode (radix).");

            exit(EXIT_FAILURE);

        }

        spapr_ovec_set(spapr->ov5_cas, OV5_MMU_RADIX_300);

    } else {

        if (kvm_enabled() && kvmppc_has_cap_mmu_radix()

            && !kvmppc_has_cap_mmu_hash_v3()) {

            error_report("Guest requested unavailable MMU mode (hash).");

            exit(EXIT_FAILURE);

        }

    }



    if (!spapr->cas_reboot) {

        spapr->cas_reboot =

            (spapr_h_cas_compose_response(spapr, args[1], args[2],

                                          ov5_updates) != 0);

    }

    spapr_ovec_cleanup(ov5_updates);



    if (spapr->cas_reboot) {

        qemu_system_reset_request();

    } else {

        

        if ((spapr->patb_entry & PATBE1_GR) && !guest_radix) {

            

            spapr_setup_hpt_and_vrma(spapr);

        }

    }



    return H_SUCCESS;

}