static int vfio_add_std_cap(VFIOPCIDevice *vdev, uint8_t pos)

{

    PCIDevice *pdev = &vdev->pdev;

    uint8_t cap_id, next, size;

    int ret;



    cap_id = pdev->config[pos];

    next = pdev->config[pos + PCI_CAP_LIST_NEXT];



    

    size = vfio_std_cap_max_size(pdev, pos);



    

    if (next) {

        ret = vfio_add_std_cap(vdev, next);

        if (ret) {

            return ret;

        }

    } else {

        

        pdev->config[PCI_CAPABILITY_LIST] = 0;

        vdev->emulated_config_bits[PCI_CAPABILITY_LIST] = 0xff;

        vdev->emulated_config_bits[PCI_STATUS] |= PCI_STATUS_CAP_LIST;

    }



    

    pci_set_byte(vdev->emulated_config_bits + pos + PCI_CAP_LIST_NEXT, 0xff);



    switch (cap_id) {

    case PCI_CAP_ID_MSI:

        ret = vfio_msi_setup(vdev, pos);

        break;

    case PCI_CAP_ID_EXP:

        vfio_check_pcie_flr(vdev, pos);

        ret = vfio_setup_pcie_cap(vdev, pos, size);

        break;

    case PCI_CAP_ID_MSIX:

        ret = vfio_msix_setup(vdev, pos);

        break;

    case PCI_CAP_ID_PM:

        vfio_check_pm_reset(vdev, pos);

        vdev->pm_cap = pos;

        ret = pci_add_capability(pdev, cap_id, pos, size);

        break;

    case PCI_CAP_ID_AF:

        vfio_check_af_flr(vdev, pos);

        ret = pci_add_capability(pdev, cap_id, pos, size);

        break;

    default:

        ret = pci_add_capability(pdev, cap_id, pos, size);

        break;

    }



    if (ret < 0) {

        error_report("vfio: %04x:%02x:%02x.%x Error adding PCI capability "

                     "0x%x[0x%x]@0x%x: %d", vdev->host.domain,

                     vdev->host.bus, vdev->host.slot, vdev->host.function,

                     cap_id, size, pos, ret);

        return ret;

    }



    return 0;

}