static int vfio_add_ext_cap(VFIOPCIDevice *vdev)

{

    PCIDevice *pdev = &vdev->pdev;

    uint32_t header;

    uint16_t cap_id, next, size;

    uint8_t cap_ver;

    uint8_t *config;



    

    config = g_memdup(pdev->config, vdev->config_size);



    for (next = PCI_CONFIG_SPACE_SIZE; next;

         next = PCI_EXT_CAP_NEXT(pci_get_long(config + next))) {

        header = pci_get_long(config + next);

        cap_id = PCI_EXT_CAP_ID(header);

        cap_ver = PCI_EXT_CAP_VER(header);



        

        size = vfio_ext_cap_max_size(config, next);



        pcie_add_capability(pdev, cap_id, cap_ver, next, size);

        pci_set_long(pdev->config + next, PCI_EXT_CAP(cap_id, cap_ver, 0));



        

        pci_long_test_and_set_mask(vdev->emulated_config_bits + next,

                                   PCI_EXT_CAP_NEXT_MASK);

    }



    g_free(config);

    return 0;

}