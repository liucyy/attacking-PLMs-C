static target_ulong h_client_architecture_support(PowerPCCPU *cpu,

                                                  sPAPRMachineState *spapr,

                                                  target_ulong opcode,

                                                  target_ulong *args)

{

    

    target_ulong addr = ppc64_phys_to_real(args[0]);

    target_ulong ov_table;

    uint32_t cas_pvr;

    sPAPROptionVector *ov1_guest, *ov5_guest, *ov5_cas_old, *ov5_updates;

    bool guest_radix;

    Error *local_err = NULL;

    bool raw_mode_supported = false;



    cas_pvr = cas_check_pvr(spapr, cpu, &addr, &raw_mode_supported, &local_err);

    if (local_err) {

        error_report_err(local_err);

        return H_HARDWARE;

    }



    

    if (cpu->compat_pvr != cas_pvr) {

        ppc_set_compat_all(cas_pvr, &local_err);

        if (local_err) {

            

            if (!raw_mode_supported) {

                error_report_err(local_err);

                return H_HARDWARE;

            }

            local_err = NULL;

        }

    }



    

    ov_table = addr;



    ov1_guest = spapr_ovec_parse_vector(ov_table, 1);

    ov5_guest = spapr_ovec_parse_vector(ov_table, 5);

    if (spapr_ovec_test(ov5_guest, OV5_MMU_BOTH)) {

        error_report("guest requested hash and radix MMU, which is invalid.");

        exit(EXIT_FAILURE);

    }

    

    guest_radix = spapr_ovec_test(ov5_guest, OV5_MMU_RADIX_300);

    spapr_ovec_clear(ov5_guest, OV5_MMU_RADIX_300);



    

    if (!guest_radix && !spapr_ovec_test(ov5_guest, OV5_HPT_RESIZE)) {

        int maxshift = spapr_hpt_shift_for_ramsize(MACHINE(spapr)->maxram_size);



        if (spapr->resize_hpt == SPAPR_RESIZE_HPT_REQUIRED) {

            error_report(

                "h_client_architecture_support: Guest doesn't support HPT resizing, but resize-hpt=required");

            exit(1);

        }



        if (spapr->htab_shift < maxshift) {

            

            spapr_reallocate_hpt(spapr, maxshift, &error_fatal);

            if (kvm_enabled()) {

                

                target_ulong sdr1 = (target_ulong)(uintptr_t)spapr->htab

                    | (spapr->htab_shift - 18);

                kvmppc_update_sdr1(sdr1);

            }

        }

    }



    

    ov5_cas_old = spapr_ovec_clone(spapr->ov5_cas);



    

    spapr_ovec_clear(ov5_cas_old, OV5_MMU_RADIX_300);



    

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

    spapr->cas_legacy_guest_workaround = !spapr_ovec_test(ov1_guest,

                                                          OV1_PPC_3_00);

    if (!spapr->cas_reboot) {

        spapr->cas_reboot =

            (spapr_h_cas_compose_response(spapr, args[1], args[2],

                                          ov5_updates) != 0);

    }

    spapr_ovec_cleanup(ov5_updates);



    if (spapr->cas_reboot) {

        qemu_system_reset_request(SHUTDOWN_CAUSE_GUEST_RESET);

    } else {

        

        if ((spapr->patb_entry & PATBE1_GR) && !guest_radix) {

            

            spapr_setup_hpt_and_vrma(spapr);

        }

    }



    return H_SUCCESS;

}