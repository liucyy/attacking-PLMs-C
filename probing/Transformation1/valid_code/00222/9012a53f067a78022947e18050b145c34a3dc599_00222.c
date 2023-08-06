static void spapr_machine_reset(void)

{

    MachineState *machine = MACHINE(qdev_get_machine());

    sPAPRMachineState *spapr = SPAPR_MACHINE(machine);

    PowerPCCPU *first_ppc_cpu;

    uint32_t rtas_limit;

    hwaddr rtas_addr, fdt_addr;

    void *fdt;

    int rc;



    

    foreach_dynamic_sysbus_device(find_unknown_sysbus_device, NULL);



    spapr_caps_reset(spapr);



    first_ppc_cpu = POWERPC_CPU(first_cpu);

    if (kvm_enabled() && kvmppc_has_cap_mmu_radix() &&

        ppc_check_compat(first_ppc_cpu, CPU_POWERPC_LOGICAL_3_00, 0,

                         spapr->max_compat_pvr)) {

        

        spapr->patb_entry = PATBE1_GR;

    } else {

        spapr_setup_hpt_and_vrma(spapr);

    }



    qemu_devices_reset();



    

    object_child_foreach_recursive(object_get_root(), spapr_reset_drcs, NULL);



    spapr_clear_pending_events(spapr);



    

    rtas_limit = MIN(spapr->rma_size, RTAS_MAX_ADDR);

    rtas_addr = rtas_limit - RTAS_MAX_SIZE;

    fdt_addr = rtas_addr - FDT_MAX_SIZE;



    

    if (!spapr->cas_reboot) {

        spapr_ovec_cleanup(spapr->ov5_cas);

        spapr->ov5_cas = spapr_ovec_new();



        ppc_set_compat(first_ppc_cpu, spapr->max_compat_pvr, &error_fatal);

    }



    fdt = spapr_build_fdt(spapr, rtas_addr, spapr->rtas_size);



    spapr_load_rtas(spapr, fdt, rtas_addr);



    rc = fdt_pack(fdt);



    

    assert(rc == 0);



    if (fdt_totalsize(fdt) > FDT_MAX_SIZE) {

        error_report("FDT too big ! 0x%x bytes (max is 0x%x)",

                     fdt_totalsize(fdt), FDT_MAX_SIZE);

        exit(1);

    }



    

    qemu_fdt_dumpdtb(fdt, fdt_totalsize(fdt));

    cpu_physical_memory_write(fdt_addr, fdt, fdt_totalsize(fdt));

    g_free(fdt);



    

    first_ppc_cpu->env.gpr[3] = fdt_addr;

    first_ppc_cpu->env.gpr[5] = 0;

    first_cpu->halted = 0;

    first_ppc_cpu->env.nip = SPAPR_ENTRY_POINT;



    spapr->cas_reboot = false;

}