static bool msix_vector_masked(PCIDevice *dev, unsigned int vector, bool fmask)

{

    unsigned offset = vector * PCI_MSIX_ENTRY_SIZE;

    uint32_t *data = (uint32_t *)&dev->msix_table[offset + PCI_MSIX_ENTRY_DATA];

    

    if (xen_is_pirq_msi(*data)) {

        return false;

    }

    return fmask || dev->msix_table[offset + PCI_MSIX_ENTRY_VECTOR_CTRL] &

        PCI_MSIX_ENTRY_CTRL_MASKBIT;

}