static void spapr_phb_remove_pci_device_cb(DeviceState *dev, void *opaque)

{

    

    pci_device_reset(PCI_DEVICE(dev));

    object_unparent(OBJECT(dev));

}