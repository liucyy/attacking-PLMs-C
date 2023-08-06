static void spapr_finalize_fdt(sPAPREnvironment *spapr,

                               target_phys_addr_t fdt_addr,

                               target_phys_addr_t rtas_addr,

                               target_phys_addr_t rtas_size)

{

    int ret;

    void *fdt;

    sPAPRPHBState *phb;



    fdt = g_malloc(FDT_MAX_SIZE);



    

    _FDT((fdt_open_into(spapr->fdt_skel, fdt, FDT_MAX_SIZE)));



    ret = spapr_populate_vdevice(spapr->vio_bus, fdt);

    if (ret < 0) {

        fprintf(stderr, "couldn't setup vio devices in fdt\n");

        exit(1);

    }



    QLIST_FOREACH(phb, &spapr->phbs, list) {

        ret = spapr_populate_pci_dt(phb, PHANDLE_XICP, fdt);

    }



    if (ret < 0) {

        fprintf(stderr, "couldn't setup PCI devices in fdt\n");

        exit(1);

    }



    

    ret = spapr_rtas_device_tree_setup(fdt, rtas_addr, rtas_size);

    if (ret < 0) {

        fprintf(stderr, "Couldn't set up RTAS device tree properties\n");

    }



    

    if (nb_numa_nodes > 1) {

        ret = spapr_set_associativity(fdt, spapr);

        if (ret < 0) {

            fprintf(stderr, "Couldn't set up NUMA device tree properties\n");

        }

    }



    if (!spapr->has_graphics) {

        spapr_populate_chosen_stdout(fdt, spapr->vio_bus);

    }



    _FDT((fdt_pack(fdt)));



    if (fdt_totalsize(fdt) > FDT_MAX_SIZE) {

        hw_error("FDT too big ! 0x%x bytes (max is 0x%x)\n",

                 fdt_totalsize(fdt), FDT_MAX_SIZE);

        exit(1);

    }



    cpu_physical_memory_write(fdt_addr, fdt, fdt_totalsize(fdt));



    g_free(fdt);

}