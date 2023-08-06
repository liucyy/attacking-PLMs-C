static void test_i440fx_defaults(gconstpointer opaque)

{

    const TestData *s = opaque;

    QPCIBus *bus;

    QPCIDevice *dev;

    uint32_t value;



    bus = test_start_get_bus(s);

    dev = qpci_device_find(bus, QPCI_DEVFN(0, 0));

    g_assert(dev != NULL);



    

    g_assert_cmpint(qpci_config_readw(dev, PCI_VENDOR_ID), ==, 0x8086);

    

    g_assert_cmpint(qpci_config_readw(dev, PCI_DEVICE_ID), ==, 0x1237);

#ifndef BROKEN

    

    g_assert_cmpint(qpci_config_readw(dev, PCI_COMMAND), ==, 0x0006);

    

    g_assert_cmpint(qpci_config_readw(dev, PCI_STATUS), ==, 0x0280);

#endif

    

    g_assert_cmpint(qpci_config_readb(dev, PCI_CLASS_PROG), ==, 0x00);

    g_assert_cmpint(qpci_config_readw(dev, PCI_CLASS_DEVICE), ==, 0x0600);

    

    g_assert_cmpint(qpci_config_readb(dev, PCI_LATENCY_TIMER), ==, 0x00);

    

    g_assert_cmpint(qpci_config_readb(dev, PCI_HEADER_TYPE), ==, 0x00);

    

    g_assert_cmpint(qpci_config_readb(dev, PCI_BIST), ==, 0x00);



    

    value = qpci_config_readw(dev, 0x50); 

    if (s->num_cpus == 1) { 

        g_assert(!(value & (1 << 15)));

    } else {

        g_assert((value & (1 << 15)));

    }



    g_assert(!(value & (1 << 6))); 



    

    g_assert_cmpint(qpci_config_readb(dev, 0x52), ==, 0x00); 

    

#ifndef BROKEN

    g_assert_cmpint(qpci_config_readb(dev, 0x53), ==, 0x80); 

#endif

    

    g_assert_cmpint(qpci_config_readb(dev, 0x54), ==, 0x00); 

    

    g_assert_cmpint(qpci_config_readw(dev, 0x55), ==, 0x0000); 

#ifndef BROKEN

    

    g_assert_cmpint(qpci_config_readb(dev, 0x57), ==, 0x01); 

    

    g_assert_cmpint(qpci_config_readb(dev, 0x58), ==, 0x10); 

#endif

    

    g_assert_cmpint(qpci_config_readb(dev, 0x59), ==, 0x00); 

    g_assert_cmpint(qpci_config_readb(dev, 0x5A), ==, 0x00); 

    g_assert_cmpint(qpci_config_readb(dev, 0x5B), ==, 0x00); 

    g_assert_cmpint(qpci_config_readb(dev, 0x5C), ==, 0x00); 

    g_assert_cmpint(qpci_config_readb(dev, 0x5D), ==, 0x00); 

    g_assert_cmpint(qpci_config_readb(dev, 0x5E), ==, 0x00); 

    g_assert_cmpint(qpci_config_readb(dev, 0x5F), ==, 0x00); 

#ifndef BROKEN

    

    g_assert_cmpint(qpci_config_readb(dev, 0x60), ==, 0x01); 

    g_assert_cmpint(qpci_config_readb(dev, 0x61), ==, 0x01); 

    g_assert_cmpint(qpci_config_readb(dev, 0x62), ==, 0x01); 

    g_assert_cmpint(qpci_config_readb(dev, 0x63), ==, 0x01); 

    g_assert_cmpint(qpci_config_readb(dev, 0x64), ==, 0x01); 

    g_assert_cmpint(qpci_config_readb(dev, 0x65), ==, 0x01); 

    g_assert_cmpint(qpci_config_readb(dev, 0x66), ==, 0x01); 

    g_assert_cmpint(qpci_config_readb(dev, 0x67), ==, 0x01); 

#endif

    

    g_assert_cmpint(qpci_config_readb(dev, 0x68), ==, 0x00); 

    

    g_assert_cmpint(qpci_config_readb(dev, 0x70), ==, 0x00); 

#ifndef BROKEN

    

    g_assert_cmpint(qpci_config_readb(dev, 0x71), ==, 0x10); 

#endif

    

    g_assert_cmpint(qpci_config_readb(dev, 0x72), ==, 0x02); 

    

    g_assert_cmpint(qpci_config_readb(dev, 0x90), ==, 0x00); 

    

    g_assert_cmpint(qpci_config_readb(dev, 0x91), ==, 0x00); 

    

    g_assert_cmpint(qpci_config_readb(dev, 0x93), ==, 0x00); 





    qtest_end();

}