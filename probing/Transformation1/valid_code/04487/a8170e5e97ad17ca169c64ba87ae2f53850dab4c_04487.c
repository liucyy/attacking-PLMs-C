static uint64_t esp_pci_io_read(void *opaque, target_phys_addr_t addr,

                                unsigned int size)

{

    PCIESPState *pci = opaque;

    uint32_t ret;



    if (addr < 0x40) {

        

        ret = esp_reg_read(&pci->esp, addr >> 2);

    } else if (addr < 0x60) {

        

        ret = esp_pci_dma_read(pci, (addr - 0x40) >> 2);

    } else if (addr == 0x70) {

        

        trace_esp_pci_sbac_read(pci->sbac);

        ret = pci->sbac;

    } else {

        

        trace_esp_pci_error_invalid_read((int)addr);

        ret = 0;

    }



    

    ret >>= (addr & 3) * 8;

    ret &= ~(~(uint64_t)0 << (8 * size));



    return ret;

}