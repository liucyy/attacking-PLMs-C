static void vtd_address_space_unmap(VTDAddressSpace *as, IOMMUNotifier *n)

{

    IOMMUTLBEntry entry;

    hwaddr size;

    hwaddr start = n->start;

    hwaddr end = n->end;



    



    if (end > VTD_ADDRESS_SIZE(VTD_HOST_ADDRESS_WIDTH)) {

        

        end = VTD_ADDRESS_SIZE(VTD_HOST_ADDRESS_WIDTH);

    }



    assert(start <= end);

    size = end - start;



    if (ctpop64(size) != 1) {

        

        int n = 64 - clz64(size);

        if (n > VTD_MGAW) {

            

            n = VTD_MGAW;

        }

        size = 1ULL << n;

    }



    entry.target_as = &address_space_memory;

    

    entry.iova = n->start & ~(size - 1);

    

    entry.translated_addr = 0;

    entry.perm = IOMMU_NONE;

    entry.addr_mask = size - 1;



    trace_vtd_as_unmap_whole(pci_bus_num(as->bus),

                             VTD_PCI_SLOT(as->devfn),

                             VTD_PCI_FUNC(as->devfn),

                             entry.iova, size);



    memory_region_notify_one(n, &entry);

}