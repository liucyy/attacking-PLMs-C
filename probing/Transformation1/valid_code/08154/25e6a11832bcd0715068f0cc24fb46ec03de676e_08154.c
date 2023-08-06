static void spapr_phb_placement(sPAPRMachineState *spapr, uint32_t index,

                                uint64_t *buid, hwaddr *pio,

                                hwaddr *mmio32, hwaddr *mmio64,

                                unsigned n_dma, uint32_t *liobns, Error **errp)

{

    

    const uint64_t base_buid = 0x800000020000000ULL;

    const int max_phbs =

        (SPAPR_PCI_LIMIT - SPAPR_PCI_BASE) / SPAPR_PCI_MEM64_WIN_SIZE - 1;

    int i;



    

    QEMU_BUILD_BUG_ON((SPAPR_PCI_BASE % SPAPR_PCI_MEM64_WIN_SIZE) != 0);

    QEMU_BUILD_BUG_ON((SPAPR_PCI_LIMIT % SPAPR_PCI_MEM64_WIN_SIZE) != 0);

    QEMU_BUILD_BUG_ON((SPAPR_PCI_MEM64_WIN_SIZE % SPAPR_PCI_MEM32_WIN_SIZE) != 0);

    QEMU_BUILD_BUG_ON((SPAPR_PCI_MEM32_WIN_SIZE % SPAPR_PCI_IO_WIN_SIZE) != 0);

    

    QEMU_BUILD_BUG_ON((max_phbs * SPAPR_PCI_IO_WIN_SIZE) > SPAPR_PCI_MEM32_WIN_SIZE);

    QEMU_BUILD_BUG_ON((max_phbs * SPAPR_PCI_MEM32_WIN_SIZE) > SPAPR_PCI_MEM64_WIN_SIZE);



    if (index >= max_phbs) {

        error_setg(errp, "\"index\" for PAPR PHB is too large (max %u)",

                   max_phbs - 1);

        return;

    }



    *buid = base_buid + index;

    for (i = 0; i < n_dma; ++i) {

        liobns[i] = SPAPR_PCI_LIOBN(index, i);

    }



    *pio = SPAPR_PCI_BASE + index * SPAPR_PCI_IO_WIN_SIZE;

    *mmio32 = SPAPR_PCI_BASE + (index + 1) * SPAPR_PCI_MEM32_WIN_SIZE;

    *mmio64 = SPAPR_PCI_BASE + (index + 1) * SPAPR_PCI_MEM64_WIN_SIZE;

}