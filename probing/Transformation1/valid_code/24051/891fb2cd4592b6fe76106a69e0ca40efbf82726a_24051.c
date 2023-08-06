int usb_device_attach(USBDevice *dev)

{

    USBBus *bus = usb_bus_from_device(dev);



    if (bus->nfree == 1 && dev->port_path == NULL) {

        

        usb_create_simple(bus, "usb-hub");

    }

    return do_attach(dev);

}