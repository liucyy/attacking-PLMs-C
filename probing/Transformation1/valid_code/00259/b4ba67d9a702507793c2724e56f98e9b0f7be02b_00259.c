static int send_dma_request(int cmd, uint64_t sector, int nb_sectors,

                            PrdtEntry *prdt, int prdt_entries,

                            void(*post_exec)(QPCIDevice *dev, void *ide_base,

                                             uint64_t sector, int nb_sectors))

{

    QPCIDevice *dev;

    void *bmdma_base;

    void *ide_base;

    uintptr_t guest_prdt;

    size_t len;

    bool from_dev;

    uint8_t status;

    int flags;



    dev = get_pci_device(&bmdma_base, &ide_base);



    flags = cmd & ~0xff;

    cmd &= 0xff;



    switch (cmd) {

    case CMD_READ_DMA:

    case CMD_PACKET:

        

        from_dev = true;

        break;

    case CMD_WRITE_DMA:

        from_dev = false;

        break;

    default:

        g_assert_not_reached();

    }



    if (flags & CMDF_NO_BM) {

        qpci_config_writew(dev, PCI_COMMAND,

                           PCI_COMMAND_IO | PCI_COMMAND_MEMORY);

    }



    

    qpci_io_writeb(dev, ide_base + reg_device, 0 | LBA);



    

    qpci_io_writeb(dev, bmdma_base + bmreg_cmd, 0);

    qpci_io_writeb(dev, bmdma_base + bmreg_status, BM_STS_INTR);



    

    len = sizeof(*prdt) * prdt_entries;

    guest_prdt = guest_alloc(guest_malloc, len);

    memwrite(guest_prdt, prdt, len);

    qpci_io_writel(dev, bmdma_base + bmreg_prdt, guest_prdt);



    

    if (cmd == CMD_PACKET) {

        

        qpci_io_writeb(dev, ide_base + reg_feature, 0x01);

    } else {

        qpci_io_writeb(dev, ide_base + reg_nsectors, nb_sectors);

        qpci_io_writeb(dev, ide_base + reg_lba_low,    sector & 0xff);

        qpci_io_writeb(dev, ide_base + reg_lba_middle, (sector >> 8) & 0xff);

        qpci_io_writeb(dev, ide_base + reg_lba_high,   (sector >> 16) & 0xff);

    }



    qpci_io_writeb(dev, ide_base + reg_command, cmd);



    if (post_exec) {

        post_exec(dev, ide_base, sector, nb_sectors);

    }



    

    qpci_io_writeb(dev, bmdma_base + bmreg_cmd,

                   BM_CMD_START | (from_dev ? BM_CMD_WRITE : 0));



    if (flags & CMDF_ABORT) {

        qpci_io_writeb(dev, bmdma_base + bmreg_cmd, 0);

    }



    

    do {

        status = qpci_io_readb(dev, bmdma_base + bmreg_status);

    } while ((status & (BM_STS_ACTIVE | BM_STS_INTR)) == BM_STS_ACTIVE);



    g_assert_cmpint(get_irq(IDE_PRIMARY_IRQ), ==, !!(status & BM_STS_INTR));



    

    assert_bit_set(qpci_io_readb(dev, ide_base + reg_status), DRDY);

    assert_bit_clear(qpci_io_readb(dev, ide_base + reg_status), BSY | DRQ);



    

    g_assert(!get_irq(IDE_PRIMARY_IRQ));



    

    if (status & BM_STS_ACTIVE) {

        qpci_io_writeb(dev, bmdma_base + bmreg_cmd, 0);

    }



    free_pci_device(dev);



    return status;

}