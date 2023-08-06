static void pci_vpb_unmap(SysBusDevice *dev, target_phys_addr_t base)

{

    PCIVPBState *s = (PCIVPBState *)dev;

    

    memory_region_del_subregion(get_system_memory(), &s->mem_config);

    

    memory_region_del_subregion(get_system_memory(), &s->mem_config2);



    if (s->realview) {

        

        memory_region_del_subregion(get_system_memory(), &s->isa);

    }

}