void msi_write_config(PCIDevice *dev, uint32_t addr, uint32_t val, int len)

{

    uint16_t flags = pci_get_word(dev->config + msi_flags_off(dev));

    bool msi64bit = flags & PCI_MSI_FLAGS_64BIT;

    bool msi_per_vector_mask = flags & PCI_MSI_FLAGS_MASKBIT;

    unsigned int nr_vectors;

    uint8_t log_num_vecs;

    uint8_t log_max_vecs;

    unsigned int vector;

    uint32_t pending;



    if (!ranges_overlap(addr, len, dev->msi_cap, msi_cap_sizeof(flags))) {

        return;

    }



#ifdef MSI_DEBUG

    MSI_DEV_PRINTF(dev, "addr 0x%"PRIx32" val 0x%"PRIx32" len %d\n",

                   addr, val, len);

    MSI_DEV_PRINTF(dev, "ctrl: 0x%"PRIx16" address: 0x%"PRIx32,

                   flags,

                   pci_get_long(dev->config + msi_address_lo_off(dev)));

    if (msi64bit) {

        fprintf(stderr, " address-hi: 0x%"PRIx32,

                pci_get_long(dev->config + msi_address_hi_off(dev)));

    }

    fprintf(stderr, " data: 0x%"PRIx16,

            pci_get_word(dev->config + msi_data_off(dev, msi64bit)));

    if (flags & PCI_MSI_FLAGS_MASKBIT) {

        fprintf(stderr, " mask 0x%"PRIx32" pending 0x%"PRIx32,

                pci_get_long(dev->config + msi_mask_off(dev, msi64bit)),

                pci_get_long(dev->config + msi_pending_off(dev, msi64bit)));

    }

    fprintf(stderr, "\n");

#endif



    if (!(flags & PCI_MSI_FLAGS_ENABLE)) {

        return;

    }



    

    pci_device_deassert_intx(dev);



    

    log_num_vecs =

        (flags & PCI_MSI_FLAGS_QSIZE) >> (ffs(PCI_MSI_FLAGS_QSIZE) - 1);

    log_max_vecs =

        (flags & PCI_MSI_FLAGS_QMASK) >> (ffs(PCI_MSI_FLAGS_QMASK) - 1);

    if (log_num_vecs > log_max_vecs) {

        flags &= ~PCI_MSI_FLAGS_QSIZE;

        flags |= log_max_vecs << (ffs(PCI_MSI_FLAGS_QSIZE) - 1);

        pci_set_word(dev->config + msi_flags_off(dev), flags);

    }



    if (!msi_per_vector_mask) {

        

        return;

    }



    nr_vectors = msi_nr_vectors(flags);



    

    pending = pci_get_long(dev->config + msi_pending_off(dev, msi64bit));

    pending &= 0xffffffff >> (PCI_MSI_VECTORS_MAX - nr_vectors);

    pci_set_long(dev->config + msi_pending_off(dev, msi64bit), pending);



    

    for (vector = 0; vector < nr_vectors; ++vector) {

        if (msi_is_masked(dev, vector) || !(pending & (1U << vector))) {

            continue;

        }



        pci_long_test_and_clear_mask(

            dev->config + msi_pending_off(dev, msi64bit), 1U << vector);

        msi_notify(dev, vector);

    }

}