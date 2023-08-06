static void ahci_test_identify(AHCIQState *ahci)
{
    uint16_t buff[256];
    unsigned px;
    int rc;
    uint16_t sect_size;
    const size_t buffsize = 512;
    g_assert(ahci != NULL);
    
    
    px = ahci_port_select(ahci);
    g_test_message("Selected port %u for test", px);
    
    ahci_port_clear(ahci, px);
    
    ahci_io(ahci, px, CMD_IDENTIFY, &buff, buffsize);
    
    
    string_bswap16(&buff[10], 20);
    rc = memcmp(&buff[10], "testdisk            ", 20);
    g_assert_cmphex(rc, ==, 0);
    string_bswap16(&buff[23], 8);
    rc = memcmp(&buff[23], "version ", 8);
    g_assert_cmphex(rc, ==, 0);
}