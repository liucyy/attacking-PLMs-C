static void rtas_ibm_change_msi(sPAPREnvironment *spapr,

                                uint32_t token, uint32_t nargs,

                                target_ulong args, uint32_t nret,

                                target_ulong rets)

{

    uint32_t config_addr = rtas_ld(args, 0);

    uint64_t buid = ((uint64_t)rtas_ld(args, 1) << 32) | rtas_ld(args, 2);

    unsigned int func = rtas_ld(args, 3);

    unsigned int req_num = rtas_ld(args, 4); 

    unsigned int seq_num = rtas_ld(args, 5);

    unsigned int ret_intr_type;

    int ndev, irq;

    sPAPRPHBState *phb = NULL;

    PCIDevice *pdev = NULL;



    switch (func) {

    case RTAS_CHANGE_MSI_FN:

    case RTAS_CHANGE_FN:

        ret_intr_type = RTAS_TYPE_MSI;

        break;

    case RTAS_CHANGE_MSIX_FN:

        ret_intr_type = RTAS_TYPE_MSIX;

        break;

    default:

        fprintf(stderr, "rtas_ibm_change_msi(%u) is not implemented\n", func);

        rtas_st(rets, 0, -3); 

        return;

    }



    

    phb = find_phb(spapr, buid);

    if (phb) {

        pdev = find_dev(spapr, buid, config_addr);

    }

    if (!phb || !pdev) {

        rtas_st(rets, 0, -3); 

        return;

    }



    

    if (!req_num) {

        ndev = spapr_msicfg_find(phb, config_addr, false);

        if (ndev < 0) {

            trace_spapr_pci_msi("MSI has not been enabled", -1, config_addr);

            rtas_st(rets, 0, -1); 

            return;

        }

        trace_spapr_pci_msi("Released MSIs", ndev, config_addr);

        rtas_st(rets, 0, 0);

        rtas_st(rets, 1, 0);

        return;

    }



    



    

    ndev = spapr_msicfg_find(phb, config_addr, true);

    if (ndev >= SPAPR_MSIX_MAX_DEVS || ndev < 0) {

        fprintf(stderr, "No free entry for a new MSI device\n");

        rtas_st(rets, 0, -1); 

        return;

    }

    trace_spapr_pci_msi("Configuring MSI", ndev, config_addr);



    

    if (phb->msi_table[ndev].nvec && (req_num != phb->msi_table[ndev].nvec)) {

        

        fprintf(stderr, "Cannot reuse MSI config for device#%d", ndev);

        rtas_st(rets, 0, -1); 

        return;

    }



    

    if (!phb->msi_table[ndev].nvec) {

        irq = spapr_allocate_irq_block(req_num, false);

        if (irq < 0) {

            fprintf(stderr, "Cannot allocate MSIs for device#%d", ndev);

            rtas_st(rets, 0, -1); 

            return;

        }

        phb->msi_table[ndev].irq = irq;

        phb->msi_table[ndev].nvec = req_num;

        phb->msi_table[ndev].config_addr = config_addr;

    }



    

    spapr_msi_setmsg(pdev, phb->msi_win_addr | (ndev << 16),

                     ret_intr_type == RTAS_TYPE_MSIX, req_num);



    rtas_st(rets, 0, 0);

    rtas_st(rets, 1, req_num);

    rtas_st(rets, 2, ++seq_num);

    rtas_st(rets, 3, ret_intr_type);



    trace_spapr_pci_rtas_ibm_change_msi(func, req_num);

}