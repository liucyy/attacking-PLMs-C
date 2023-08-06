static pcibus_t pci_bar_address(PCIDevice *d,

				int reg, uint8_t type, pcibus_t size)

{

    pcibus_t new_addr, last_addr;

    int bar = pci_bar(d, reg);

    uint16_t cmd = pci_get_word(d->config + PCI_COMMAND);



    if (type & PCI_BASE_ADDRESS_SPACE_IO) {

        if (!(cmd & PCI_COMMAND_IO)) {

            return PCI_BAR_UNMAPPED;

        }

        new_addr = pci_get_long(d->config + bar) & ~(size - 1);

        last_addr = new_addr + size - 1;

        

        if (last_addr <= new_addr || new_addr == 0 || last_addr >= UINT32_MAX) {

            return PCI_BAR_UNMAPPED;

        }

        return new_addr;

    }



    if (!(cmd & PCI_COMMAND_MEMORY)) {

        return PCI_BAR_UNMAPPED;

    }

    if (type & PCI_BASE_ADDRESS_MEM_TYPE_64) {

        new_addr = pci_get_quad(d->config + bar);

    } else {

        new_addr = pci_get_long(d->config + bar);

    }

    

    if (reg == PCI_ROM_SLOT && !(new_addr & PCI_ROM_ADDRESS_ENABLE)) {

        return PCI_BAR_UNMAPPED;

    }

    new_addr &= ~(size - 1);

    last_addr = new_addr + size - 1;

    

    

    if (last_addr <= new_addr || new_addr == 0 ||

        last_addr == PCI_BAR_UNMAPPED) {

        return PCI_BAR_UNMAPPED;

    }



    

    if  (!(type & PCI_BASE_ADDRESS_MEM_TYPE_64) && last_addr >= UINT32_MAX) {

        return PCI_BAR_UNMAPPED;

    }



    

    if (last_addr >= HWADDR_MAX) {

        return PCI_BAR_UNMAPPED;

    }



    return new_addr;

}