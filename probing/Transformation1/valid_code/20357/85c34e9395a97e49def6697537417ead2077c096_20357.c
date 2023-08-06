static void ahci_test_identify(AHCIQState *ahci)

{

    RegD2HFIS *d2h = g_malloc0(0x20);

    RegD2HFIS *pio = g_malloc0(0x20);

    RegH2DFIS fis;

    AHCICommandHeader cmd;

    PRD prd;

    uint32_t reg, data_ptr;

    uint16_t buff[256];

    unsigned i;

    int rc;

    uint8_t cx;

    uint64_t table;



    g_assert(ahci != NULL);



    



    

    i = ahci_port_select(ahci);

    g_test_message("Selected port %u for test", i);



    

    ahci_port_clear(ahci, i);



    

    

    table = ahci_alloc(ahci, CMD_TBL_SIZ(1));

    g_assert(table);

    ASSERT_BIT_CLEAR(table, 0x7F);



    

    data_ptr = ahci_alloc(ahci, 512);

    g_assert(data_ptr);



    

    cx = ahci_pick_cmd(ahci, i);



    

    memset(&cmd, 0x00, sizeof(cmd));

    cmd.flags = 5;      

    cmd.flags |= 0x400; 

    cmd.prdtl = 1;      

    cmd.prdbc = 0;

    cmd.ctba = table;



    

    prd.dba = cpu_to_le64(data_ptr);

    prd.res = 0;

    

    prd.dbc = cpu_to_le32(511 | 0x80000000);



    

    memset(&fis, 0x00, sizeof(fis));

    fis.fis_type = 0x27; 

    fis.command = 0xEC;  

    fis.device = 0;

    fis.flags = 0x80;    



    

    g_assert_cmphex(ahci_px_rreg(ahci, i, AHCI_PX_IS), ==, 0);



    

    memwrite(table, &fis, sizeof(fis));



    

    memwrite(table + 0x80, &prd, sizeof(prd));



    

    ahci_set_command_header(ahci, i, cx, &cmd);



    

    g_assert_cmphex(ahci_px_rreg(ahci, i, AHCI_PX_IS), ==, 0);



    

    ahci_px_wreg(ahci, i, AHCI_PX_CI, (1 << cx));

    while (BITSET(ahci_px_rreg(ahci, i, AHCI_PX_TFD), AHCI_PX_TFD_STS_BSY)) {

        usleep(50);

    }



    

    reg = ahci_px_rreg(ahci, i, AHCI_PX_IS);

    ASSERT_BIT_SET(reg, AHCI_PX_IS_DHRS);

    ASSERT_BIT_SET(reg, AHCI_PX_IS_PSS);

    

    ASSERT_BIT_CLEAR(reg, AHCI_PX_IS_DPS);



    

    ahci_px_wreg(ahci, i, AHCI_PX_IS,

                 AHCI_PX_IS_DHRS | AHCI_PX_IS_PSS | AHCI_PX_IS_DPS);

    g_assert_cmphex(ahci_px_rreg(ahci, i, AHCI_PX_IS), ==, 0);



    

    reg = ahci_px_rreg(ahci, i, AHCI_PX_SERR);

    g_assert_cmphex(reg, ==, 0);

    reg = ahci_px_rreg(ahci, i, AHCI_PX_TFD);

    ASSERT_BIT_CLEAR(reg, AHCI_PX_TFD_STS_ERR);

    ASSERT_BIT_CLEAR(reg, AHCI_PX_TFD_ERR);



    

    ahci_get_command_header(ahci, i, cx, &cmd);

    g_assert_cmphex(512, ==, cmd.prdbc);



    

    memread(ahci->port[i].fb + 0x20, pio, 0x20);

    memread(ahci->port[i].fb + 0x40, d2h, 0x20);

    g_assert_cmphex(pio->fis_type, ==, 0x5f);

    g_assert_cmphex(d2h->fis_type, ==, 0x34);

    g_assert_cmphex(pio->flags, ==, d2h->flags);

    g_assert_cmphex(pio->status, ==, d2h->status);

    g_assert_cmphex(pio->error, ==, d2h->error);



    reg = ahci_px_rreg(ahci, i, AHCI_PX_TFD);

    g_assert_cmphex((reg & AHCI_PX_TFD_ERR), ==, pio->error);

    g_assert_cmphex((reg & AHCI_PX_TFD_STS), ==, pio->status);

    

    g_assert_cmphex(le16_to_cpu(pio->res4), ==, cmd.prdbc);



    

    memread(data_ptr, &buff, 512);



    

    

    string_bswap16(&buff[10], 20);

    rc = memcmp(&buff[10], "testdisk            ", 20);

    g_assert_cmphex(rc, ==, 0);



    string_bswap16(&buff[23], 8);

    rc = memcmp(&buff[23], "version ", 8);

    g_assert_cmphex(rc, ==, 0);



    g_free(d2h);

    g_free(pio);

}