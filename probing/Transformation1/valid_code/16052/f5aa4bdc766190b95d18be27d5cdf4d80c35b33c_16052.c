static void test_flush(void)

{

    QPCIDevice *dev;

    QPCIBar bmdma_bar, ide_bar;

    uint8_t data;



    ide_test_start(

        "-drive file=blkdebug::%s,if=ide,cache=writeback,format=raw",

        tmp_path);



    dev = get_pci_device(&bmdma_bar, &ide_bar);



    qtest_irq_intercept_in(global_qtest, "ioapic");



    

    make_dirty(0);



    

    g_free(hmp("qemu-io ide0-hd0 \"break flush_to_os A\""));



    

    qpci_io_writeb(dev, ide_bar, reg_device, 0);

    qpci_io_writeb(dev, ide_bar, reg_command, CMD_FLUSH_CACHE);



    

    data = qpci_io_readb(dev, ide_bar, reg_status);

    assert_bit_set(data, BSY | DRDY);

    assert_bit_clear(data, DF | ERR | DRQ);



    

    g_free(hmp("qemu-io ide0-hd0 \"resume A\""));



    

    data = qpci_io_readb(dev, ide_bar, reg_device);

    g_assert_cmpint(data & DEV, ==, 0);



    do {

        data = qpci_io_readb(dev, ide_bar, reg_status);

    } while (data & BSY);



    assert_bit_set(data, DRDY);

    assert_bit_clear(data, BSY | DF | ERR | DRQ);



    ide_test_quit();


}