static void test_smram_lock(void)
{
    QPCIBus *pcibus;
    QPCIDevice *pcidev;
    QDict *response;
    pcibus = qpci_init_pc(NULL);
    g_assert(pcibus != NULL);
    pcidev = qpci_device_find(pcibus, 0);
    g_assert(pcidev != NULL);
    
    smram_set_bit(pcidev, MCH_HOST_BRIDGE_SMRAM_D_OPEN, false);
    g_assert(smram_test_bit(pcidev, MCH_HOST_BRIDGE_SMRAM_D_OPEN) == false);
    smram_set_bit(pcidev, MCH_HOST_BRIDGE_SMRAM_D_OPEN, true);
    g_assert(smram_test_bit(pcidev, MCH_HOST_BRIDGE_SMRAM_D_OPEN) == true);
    
    smram_set_bit(pcidev, MCH_HOST_BRIDGE_SMRAM_D_LCK, true);
    g_assert(smram_test_bit(pcidev, MCH_HOST_BRIDGE_SMRAM_D_OPEN) == false);
    smram_set_bit(pcidev, MCH_HOST_BRIDGE_SMRAM_D_OPEN, true);
    g_assert(smram_test_bit(pcidev, MCH_HOST_BRIDGE_SMRAM_D_OPEN) == false);
    
    response = qmp("{'execute': 'system_reset', 'arguments': {} }");
    g_assert(response);
    g_assert(!qdict_haskey(response, "error"));
    QDECREF(response);
    
    smram_set_bit(pcidev, MCH_HOST_BRIDGE_SMRAM_D_OPEN, false);
    g_assert(smram_test_bit(pcidev, MCH_HOST_BRIDGE_SMRAM_D_OPEN) == false);
    smram_set_bit(pcidev, MCH_HOST_BRIDGE_SMRAM_D_OPEN, true);
    g_assert(smram_test_bit(pcidev, MCH_HOST_BRIDGE_SMRAM_D_OPEN) == true);
}