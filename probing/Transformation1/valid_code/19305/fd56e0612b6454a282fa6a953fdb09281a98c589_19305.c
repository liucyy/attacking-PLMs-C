static int vfio_setup_pcie_cap(VFIOPCIDevice *vdev, int pos, uint8_t size,

                               Error **errp)

{

    uint16_t flags;

    uint8_t type;



    flags = pci_get_word(vdev->pdev.config + pos + PCI_CAP_FLAGS);

    type = (flags & PCI_EXP_FLAGS_TYPE) >> 4;



    if (type != PCI_EXP_TYPE_ENDPOINT &&

        type != PCI_EXP_TYPE_LEG_END &&

        type != PCI_EXP_TYPE_RC_END) {



        error_setg(errp, "assignment of PCIe type 0x%x "

                   "devices is not currently supported", type);

        return -EINVAL;

    }



    if (!pci_bus_is_express(vdev->pdev.bus)) {

        PCIBus *bus = vdev->pdev.bus;

        PCIDevice *bridge;



        

        while (!pci_bus_is_root(bus)) {

            bridge = pci_bridge_get_device(bus);

            bus = bridge->bus;

        }



        if (pci_bus_is_express(bus)) {

            return 0;

        }



    } else if (pci_bus_is_root(vdev->pdev.bus)) {

        

        if (type == PCI_EXP_TYPE_ENDPOINT) {

            vfio_add_emulated_word(vdev, pos + PCI_CAP_FLAGS,

                                   PCI_EXP_TYPE_RC_END << 4,

                                   PCI_EXP_FLAGS_TYPE);



            

            if (size > PCI_EXP_LNKCTL) {

                vfio_add_emulated_long(vdev, pos + PCI_EXP_LNKCAP, 0, ~0);

                vfio_add_emulated_word(vdev, pos + PCI_EXP_LNKCTL, 0, ~0);

                vfio_add_emulated_word(vdev, pos + PCI_EXP_LNKSTA, 0, ~0);



#ifndef PCI_EXP_LNKCAP2

#define PCI_EXP_LNKCAP2 44

#endif

#ifndef PCI_EXP_LNKSTA2

#define PCI_EXP_LNKSTA2 50

#endif

                

                if (size > PCI_EXP_LNKCAP2) {

                    vfio_add_emulated_long(vdev, pos + PCI_EXP_LNKCAP2, 0, ~0);

                    vfio_add_emulated_word(vdev, pos + PCI_EXP_LNKCTL2, 0, ~0);

                    vfio_add_emulated_word(vdev, pos + PCI_EXP_LNKSTA2, 0, ~0);

                }

            }



        } else if (type == PCI_EXP_TYPE_LEG_END) {

            

            return 0;

        }



    } else {

        

        if (type == PCI_EXP_TYPE_RC_END) {

            vfio_add_emulated_word(vdev, pos + PCI_CAP_FLAGS,

                                   PCI_EXP_TYPE_ENDPOINT << 4,

                                   PCI_EXP_FLAGS_TYPE);

            vfio_add_emulated_long(vdev, pos + PCI_EXP_LNKCAP,

                                   PCI_EXP_LNK_MLW_1 | PCI_EXP_LNK_LS_25, ~0);

            vfio_add_emulated_word(vdev, pos + PCI_EXP_LNKCTL, 0, ~0);

        }



        

        vfio_add_emulated_word(vdev, pos + PCI_EXP_LNKSTA,

                               pci_get_word(vdev->pdev.config + pos +

                                            PCI_EXP_LNKSTA),

                               PCI_EXP_LNKCAP_MLW | PCI_EXP_LNKCAP_SLS);

    }



    

    if ((flags & PCI_EXP_FLAGS_VERS) == 0) {

        vfio_add_emulated_word(vdev, pos + PCI_CAP_FLAGS,

                               1, PCI_EXP_FLAGS_VERS);

    }



    pos = pci_add_capability(&vdev->pdev, PCI_CAP_ID_EXP, pos, size,

                             errp);

    if (pos < 0) {

        return pos;

    }



    vdev->pdev.exp.exp_cap = pos;



    return pos;

}