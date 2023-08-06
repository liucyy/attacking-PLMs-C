static void pci_vpb_map(SysBusDevice *dev, target_phys_addr_t base)

{

    PCIVPBState *s = (PCIVPBState *)dev;

    

    memory_region_add_subregion(get_system_memory(), base + 0x01000000,

                                &s->mem_config);

    

    memory_region_add_subregion(get_system_memory(), base + 0x02000000,

                                &s->mem_config2);



    if (s->realview) {

        

        memory_region_add_subregion(get_system_memory(), base + 0x03000000,

                                    &s->isa);

    }

}