static char *spapr_get_fw_dev_path(FWPathProvider *p, BusState *bus,

                                   DeviceState *dev)

{

#define CAST(type, obj, name) \

    ((type *)object_dynamic_cast(OBJECT(obj), (name)))

    SCSIDevice *d = CAST(SCSIDevice,  dev, TYPE_SCSI_DEVICE);

    sPAPRPHBState *phb = CAST(sPAPRPHBState, dev, TYPE_SPAPR_PCI_HOST_BRIDGE);

    VHostSCSICommon *vsc = CAST(VHostSCSICommon, dev, TYPE_VHOST_SCSI_COMMON);



    if (d) {

        void *spapr = CAST(void, bus->parent, "spapr-vscsi");

        VirtIOSCSI *virtio = CAST(VirtIOSCSI, bus->parent, TYPE_VIRTIO_SCSI);

        USBDevice *usb = CAST(USBDevice, bus->parent, TYPE_USB_DEVICE);



        if (spapr) {

            

            unsigned id = 0x8000 | (d->id << 8) | d->lun;

            return g_strdup_printf("%s@%"PRIX64, qdev_fw_name(dev),

                                   (uint64_t)id << 48);

        } else if (virtio) {

            

            unsigned id = 0x1000000 | (d->id << 16) | d->lun;





            return g_strdup_printf("%s@%"PRIX64, qdev_fw_name(dev),

                                   (uint64_t)id << 32);

        } else if (usb) {

            

            unsigned usb_port = atoi(usb->port->path);

            unsigned id = 0x1000000 | (usb_port << 16) | d->lun;

            return g_strdup_printf("%s@%"PRIX64, qdev_fw_name(dev),

                                   (uint64_t)id << 32);





    

    if (strcmp("usb-host", qdev_fw_name(dev)) == 0) {

        USBDevice *usbdev = CAST(USBDevice, dev, TYPE_USB_DEVICE);

        if (usb_host_dev_is_scsi_storage(usbdev)) {

            return g_strdup_printf("storage@%s/disk", usbdev->port->path);





    if (phb) {

        

        return g_strdup_printf("pci@%"PRIX64, phb->buid);




    if (vsc) {

        

        unsigned id = 0x1000000 | (vsc->target << 16) | vsc->lun;

        return g_strdup_printf("disk@%"PRIX64, (uint64_t)id << 32);




    if (g_str_equal("pci-bridge", qdev_fw_name(dev))) {

        

        PCIDevice *pcidev = CAST(PCIDevice, dev, TYPE_PCI_DEVICE);

        return g_strdup_printf("pci@%x", PCI_SLOT(pcidev->devfn));




    return NULL;