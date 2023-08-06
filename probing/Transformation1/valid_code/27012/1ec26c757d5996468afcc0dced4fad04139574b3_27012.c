static target_ulong h_resize_hpt_commit(PowerPCCPU *cpu,

                                        sPAPRMachineState *spapr,

                                        target_ulong opcode,

                                        target_ulong *args)

{

    target_ulong flags = args[0];

    target_ulong shift = args[1];

    sPAPRPendingHPT *pending = spapr->pending_hpt;

    int rc;

    size_t newsize;



    if (spapr->resize_hpt == SPAPR_RESIZE_HPT_DISABLED) {

        return H_AUTHORITY;

    }



    trace_spapr_h_resize_hpt_commit(flags, shift);



    rc = kvmppc_resize_hpt_commit(cpu, flags, shift);

    if (rc != -ENOSYS) {

        return resize_hpt_convert_rc(rc);

    }



    if (flags != 0) {

        return H_PARAMETER;

    }



    if (!pending || (pending->shift != shift)) {

        

        return H_CLOSED;

    }



    if (!pending->complete) {

        

        return H_BUSY;

    }



    

    g_assert(spapr->htab_shift);



    newsize = 1ULL << pending->shift;

    rc = rehash_hpt(cpu, spapr->htab, HTAB_SIZE(spapr),

                    pending->hpt, newsize);

    if (rc == H_SUCCESS) {

        qemu_vfree(spapr->htab);

        spapr->htab = pending->hpt;

        spapr->htab_shift = pending->shift;



        if (kvm_enabled()) {

            

            target_ulong sdr1 = (target_ulong)(uintptr_t)spapr->htab

                | (spapr->htab_shift - 18);

            kvmppc_update_sdr1(sdr1);

        }



        pending->hpt = NULL; 

    }



    

    spapr->pending_hpt = NULL;

    free_pending_hpt(pending);



    return rc;

}