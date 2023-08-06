static QPCIDevice *start_ahci_device(QPCIDevice *ahci, void **hba_base)

{

    

    *hba_base = qpci_iomap(ahci, 5, NULL);



    

    qpci_device_enable(ahci);



    return ahci;

}