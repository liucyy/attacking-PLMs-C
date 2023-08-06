static void pci_bridge_update_mappings(PCIBridge *br)

{

    

    memory_region_transaction_begin();

    pci_bridge_region_cleanup(br);

    pci_bridge_region_init(br);

    memory_region_transaction_commit();

}