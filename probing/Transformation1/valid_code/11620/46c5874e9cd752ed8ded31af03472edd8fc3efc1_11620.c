static void finish_read_pci_config(sPAPREnvironment *spapr, uint64_t buid,

                                   uint32_t addr, uint32_t size,

                                   target_ulong rets)

{

    PCIDevice *pci_dev;

    uint32_t val;



    if ((size != 1) && (size != 2) && (size != 4)) {

        

        rtas_st(rets, 0, RTAS_OUT_HW_ERROR);

        return;

    }



    pci_dev = find_dev(spapr, buid, addr);

    addr = rtas_pci_cfgaddr(addr);



    if (!pci_dev || (addr % size) || (addr >= pci_config_size(pci_dev))) {

        

        rtas_st(rets, 0, RTAS_OUT_HW_ERROR);

        return;

    }



    val = pci_host_config_read_common(pci_dev, addr,

                                      pci_config_size(pci_dev), size);



    rtas_st(rets, 0, RTAS_OUT_SUCCESS);

    rtas_st(rets, 1, val);

}