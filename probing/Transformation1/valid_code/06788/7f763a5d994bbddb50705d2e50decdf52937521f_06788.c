static void ppc_spapr_reset(void)

{

    

    memset(spapr->htab, 0, spapr->htab_size);



    qemu_devices_reset();



    

    spapr_finalize_fdt(spapr, spapr->fdt_addr, spapr->rtas_addr,

                       spapr->rtas_size);



    

    first_cpu->gpr[3] = spapr->fdt_addr;

    first_cpu->gpr[5] = 0;

    first_cpu->halted = 0;

    first_cpu->nip = spapr->entry_point;



}