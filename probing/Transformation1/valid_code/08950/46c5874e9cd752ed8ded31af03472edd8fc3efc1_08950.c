static void rtas_ibm_change_msi(PowerPCCPU *cpu, sPAPREnvironment *spapr,

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

    unsigned int irq, max_irqs = 0, num = 0;

    sPAPRPHBState *phb = NULL;

    PCIDevice *pdev = NULL;

    spapr_pci_msi *msi;

    int *config_addr_key;



    switch (func) {

    case RTAS_CHANGE_MSI_FN:

    case RTAS_CHANGE_FN:

        ret_intr_type = RTAS_TYPE_MSI;

        break;

    case RTAS_CHANGE_MSIX_FN:

        ret_intr_type = RTAS_TYPE_MSIX;

        break;

    default:

        error_report("rtas_ibm_change_msi(%u) is not implemented", func);

        rtas_st(rets, 0, RTAS_OUT_PARAM_ERROR);

        return;

    }



    

    phb = find_phb(spapr, buid);

    if (phb) {

        pdev = find_dev(spapr, buid, config_addr);

    }

    if (!phb || !pdev) {

        rtas_st(rets, 0, RTAS_OUT_PARAM_ERROR);

        return;

    }



    

    if (!req_num) {

        msi = (spapr_pci_msi *) g_hash_table_lookup(phb->msi, &config_addr);

        if (!msi) {

            trace_spapr_pci_msi("Releasing wrong config", config_addr);

            rtas_st(rets, 0, RTAS_OUT_HW_ERROR);

            return;

        }



        xics_free(spapr->icp, msi->first_irq, msi->num);

        if (msi_present(pdev)) {

            spapr_msi_setmsg(pdev, 0, false, 0, num);

        }

        if (msix_present(pdev)) {

            spapr_msi_setmsg(pdev, 0, true, 0, num);

        }

        g_hash_table_remove(phb->msi, &config_addr);



        trace_spapr_pci_msi("Released MSIs", config_addr);

        rtas_st(rets, 0, RTAS_OUT_SUCCESS);

        rtas_st(rets, 1, 0);

        return;

    }



    



    

    if (ret_intr_type == RTAS_TYPE_MSI) {

        max_irqs = msi_nr_vectors_allocated(pdev);

    } else if (ret_intr_type == RTAS_TYPE_MSIX) {

        max_irqs = pdev->msix_entries_nr;

    }

    if (!max_irqs) {

        error_report("Requested interrupt type %d is not enabled for device %x",

                     ret_intr_type, config_addr);

        rtas_st(rets, 0, -1); 

        return;

    }

    

    if (req_num > max_irqs) {

        trace_spapr_pci_msi_retry(config_addr, req_num, max_irqs);

        req_num = max_irqs;

        irq = 0; 

        goto out;

    }



    

    irq = xics_alloc_block(spapr->icp, 0, req_num, false,

                           ret_intr_type == RTAS_TYPE_MSI);

    if (!irq) {

        error_report("Cannot allocate MSIs for device %x", config_addr);

        rtas_st(rets, 0, RTAS_OUT_HW_ERROR);

        return;

    }



    

    spapr_msi_setmsg(pdev, SPAPR_PCI_MSI_WINDOW, ret_intr_type == RTAS_TYPE_MSIX,

                     irq, req_num);



    

    msi = g_new(spapr_pci_msi, 1);

    msi->first_irq = irq;

    msi->num = req_num;

    config_addr_key = g_new(int, 1);

    *config_addr_key = config_addr;

    g_hash_table_insert(phb->msi, config_addr_key, msi);



out:

    rtas_st(rets, 0, RTAS_OUT_SUCCESS);

    rtas_st(rets, 1, req_num);

    rtas_st(rets, 2, ++seq_num);

    rtas_st(rets, 3, ret_intr_type);



    trace_spapr_pci_rtas_ibm_change_msi(config_addr, func, req_num, irq);

}