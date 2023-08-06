static void free_ahci_device(QPCIDevice *dev)

{

    QPCIBus *pcibus = dev ? dev->bus : NULL;



    

    g_free(dev);

    qpci_free_pc(pcibus);

}