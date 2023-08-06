void pc_init_pci64_hole(PcPciInfo *pci_info, uint64_t pci_hole64_start,

                        uint64_t pci_hole64_size)

{

    if ((sizeof(hwaddr) == 4) || (!pci_hole64_size)) {

        return;

    }

    

    pci_info->w64.begin = ROUND_UP(pci_hole64_start, 0x1ULL << 30);

    pci_info->w64.end = pci_info->w64.begin + pci_hole64_size;

    assert(pci_info->w64.begin <= pci_info->w64.end);

}