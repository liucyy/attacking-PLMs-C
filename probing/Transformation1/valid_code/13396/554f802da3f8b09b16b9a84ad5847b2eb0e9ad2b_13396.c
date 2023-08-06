void pcie_cap_slot_hotplug_cb(HotplugHandler *hotplug_dev, DeviceState *dev,

                              Error **errp)

{

    uint8_t *exp_cap;

    PCIDevice *pci_dev = PCI_DEVICE(dev);



    pcie_cap_slot_hotplug_common(PCI_DEVICE(hotplug_dev), dev, &exp_cap, errp);



    

    if (!dev->hotplugged) {

        pci_word_test_and_set_mask(exp_cap + PCI_EXP_SLTSTA,

                                   PCI_EXP_SLTSTA_PDS);

        return;

    }



    

    assert(PCI_FUNC(pci_dev->devfn) == 0);



    pci_word_test_and_set_mask(exp_cap + PCI_EXP_SLTSTA,

                               PCI_EXP_SLTSTA_PDS);

    pcie_cap_slot_event(PCI_DEVICE(hotplug_dev), PCI_EXP_HP_EV_PDC);

}