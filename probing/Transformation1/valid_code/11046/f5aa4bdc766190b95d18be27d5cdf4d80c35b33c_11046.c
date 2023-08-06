static void test_flush_nodev(void)

{

    QPCIDevice *dev;

    QPCIBar bmdma_bar, ide_bar;



    ide_test_start("");



    dev = get_pci_device(&bmdma_bar, &ide_bar);



    

    qpci_io_writeb(dev, ide_bar, reg_device, 0);

    qpci_io_writeb(dev, ide_bar, reg_command, CMD_FLUSH_CACHE);



    




    ide_test_quit();

}