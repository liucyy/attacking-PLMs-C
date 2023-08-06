static int pcibus_reset(BusState *qbus)

{

    pci_bus_reset(DO_UPCAST(PCIBus, qbus, qbus));



    

    return 1;

}