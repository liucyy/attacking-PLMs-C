static void start_ahci_device(AHCIQState *ahci)

{

    

    ahci->hba_base = qpci_iomap(ahci->dev, 5, &ahci->barsize);



    

    qpci_device_enable(ahci->dev);

}