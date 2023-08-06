void memory_region_iommu_replay(MemoryRegion *mr, Notifier *n,

                                hwaddr granularity, bool is_write)

{

    hwaddr addr;

    IOMMUTLBEntry iotlb;



    for (addr = 0; addr < memory_region_size(mr); addr += granularity) {

        iotlb = mr->iommu_ops->translate(mr, addr, is_write);

        if (iotlb.perm != IOMMU_NONE) {

            n->notify(n, &iotlb);

        }



        

        if ((addr + granularity) < addr) {

            break;

        }

    }

}