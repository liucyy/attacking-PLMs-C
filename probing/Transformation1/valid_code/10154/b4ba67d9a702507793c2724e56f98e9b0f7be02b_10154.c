static void test_bmdma_no_busmaster(void)

{

    QPCIDevice *dev;

    void *bmdma_base, *ide_base;

    uint8_t status;



    dev = get_pci_device(&bmdma_base, &ide_base);



    

    PrdtEntry prdt[4096] = { };



    status = send_dma_request(CMD_READ_DMA | CMDF_NO_BM, 0, 512,

                              prdt, ARRAY_SIZE(prdt), NULL);



    

    g_assert_cmphex(status, ==, BM_STS_ACTIVE | BM_STS_INTR);

    assert_bit_clear(qpci_io_readb(dev, ide_base + reg_status), DF | ERR);

}