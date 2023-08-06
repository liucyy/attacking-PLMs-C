static void vfio_add_ext_cap(VFIOPCIDevice *vdev)

{

    PCIDevice *pdev = &vdev->pdev;

    uint32_t header;

    uint16_t cap_id, next, size;

    uint8_t cap_ver;

    uint8_t *config;



    

    if (!pci_is_express(pdev) || !pci_bus_is_express(pdev->bus) ||

        !pci_get_long(pdev->config + PCI_CONFIG_SPACE_SIZE)) {

        return;

    }



    

    config = g_memdup(pdev->config, vdev->config_size);



    

    pci_set_long(pdev->config + PCI_CONFIG_SPACE_SIZE,

                 PCI_EXT_CAP(0xFFFF, 0, 0));

    pci_set_long(pdev->wmask + PCI_CONFIG_SPACE_SIZE, 0);

    pci_set_long(vdev->emulated_config_bits + PCI_CONFIG_SPACE_SIZE, ~0);



    for (next = PCI_CONFIG_SPACE_SIZE; next;

         next = PCI_EXT_CAP_NEXT(pci_get_long(config + next))) {

        header = pci_get_long(config + next);

        cap_id = PCI_EXT_CAP_ID(header);

        cap_ver = PCI_EXT_CAP_VER(header);



        

        size = vfio_ext_cap_max_size(config, next);



        

        pci_long_test_and_set_mask(vdev->emulated_config_bits + next,

                                   PCI_EXT_CAP_NEXT_MASK);



        switch (cap_id) {

        case PCI_EXT_CAP_ID_SRIOV: 

        case PCI_EXT_CAP_ID_ARI: 

            trace_vfio_add_ext_cap_dropped(vdev->vbasedev.name, cap_id, next);

            break;

        default:

            pcie_add_capability(pdev, cap_id, cap_ver, next, size);

        }



    }



    

    if (pci_get_word(pdev->config + PCI_CONFIG_SPACE_SIZE) == 0xFFFF) {

        pci_set_word(pdev->config + PCI_CONFIG_SPACE_SIZE, 0);

    }



    g_free(config);

    return;

}