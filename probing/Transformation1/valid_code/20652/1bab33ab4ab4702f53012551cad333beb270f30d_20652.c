static void test_i440fx_pam(gconstpointer opaque)
{
    const TestData *s = opaque;
    QPCIBus *bus;
    QPCIDevice *dev;
    int i;
    static struct {
        uint32_t start;
        uint32_t end;
    } pam_area[] = {
        { 0, 0 },             
        { 0xF0000, 0xFFFFF }, 
        { 0xC0000, 0xC3FFF }, 
        { 0xC4000, 0xC7FFF }, 
        { 0xC8000, 0xCBFFF }, 
        { 0xCC000, 0xCFFFF }, 
        { 0xD0000, 0xD3FFF }, 
        { 0xD4000, 0xD7FFF }, 
        { 0xD8000, 0xDBFFF }, 
        { 0xDC000, 0xDFFFF }, 
        { 0xE0000, 0xE3FFF }, 
        { 0xE4000, 0xE7FFF }, 
        { 0xE8000, 0xEBFFF }, 
        { 0xEC000, 0xEFFFF }, 
    };
    bus = test_start_get_bus(s);
    dev = qpci_device_find(bus, QPCI_DEVFN(0, 0));
    g_assert(dev != NULL);
    for (i = 0; i < ARRAY_SIZE(pam_area); i++) {
        if (pam_area[i].start == pam_area[i].end) {
            continue;
        }
        g_test_message("Checking area 0x%05x..0x%05x",
                       pam_area[i].start, pam_area[i].end);
        
        pam_set(dev, i, PAM_RE);
        
        g_assert(verify_area(pam_area[i].start, pam_area[i].end, 0));
        
        pam_set(dev, i, PAM_RE | PAM_WE);
        
        write_area(pam_area[i].start, pam_area[i].end, 0x42);
#ifndef BROKEN
        
        
        pam_set(dev, i, PAM_WE);
        
        g_assert(!verify_area(pam_area[i].start, pam_area[i].end, 0x42));
#endif
        
        g_assert(verify_area(pam_area[i].start, pam_area[i].end, 0x42));
        
        write_area(pam_area[i].start, pam_area[i].end, 0x82);
#ifndef BROKEN
        
        
        g_assert(!verify_area(pam_area[i].start, pam_area[i].end, 0x82));
        
        pam_set(dev, i, PAM_RE | PAM_WE);
#endif
        
        g_assert(verify_area(pam_area[i].start, pam_area[i].end, 0x82));
        
        pam_set(dev, i, 0);
        
        g_assert(!verify_area(pam_area[i].start, pam_area[i].end, 0x82));
    }
    qtest_end();
}