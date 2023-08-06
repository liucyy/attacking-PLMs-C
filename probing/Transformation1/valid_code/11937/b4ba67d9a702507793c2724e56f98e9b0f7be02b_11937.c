void start_ahci_device(AHCIQState *ahci)

{

    

    ahci->hba_base = qpci_iomap(ahci->dev, 5, &ahci->barsize);

    g_assert(ahci->hba_base);



    

    qpci_device_enable(ahci->dev);

}