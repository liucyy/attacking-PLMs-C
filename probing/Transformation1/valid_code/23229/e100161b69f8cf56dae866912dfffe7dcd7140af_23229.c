int vfio_spapr_create_window(VFIOContainer *container,

                             MemoryRegionSection *section,

                             hwaddr *pgsize)

{

    int ret;

    IOMMUMemoryRegion *iommu_mr = IOMMU_MEMORY_REGION(section->mr);

    unsigned pagesize = memory_region_iommu_get_min_page_size(iommu_mr);

    unsigned entries, pages;

    struct vfio_iommu_spapr_tce_create create = { .argsz = sizeof(create) };



    

    create.window_size = int128_get64(section->size);

    create.page_shift = ctz64(pagesize);

    

    entries = create.window_size >> create.page_shift;

    pages = MAX((entries * sizeof(uint64_t)) / getpagesize(), 1);

    pages = MAX(pow2ceil(pages) - 1, 1); 

    create.levels = ctz64(pages) / 6 + 1;



    ret = ioctl(container->fd, VFIO_IOMMU_SPAPR_TCE_CREATE, &create);

    if (ret) {

        error_report("Failed to create a window, ret = %d (%m)", ret);

        return -errno;

    }



    if (create.start_addr != section->offset_within_address_space) {

        vfio_spapr_remove_window(container, create.start_addr);



        error_report("Host doesn't support DMA window at %"HWADDR_PRIx", must be %"PRIx64,

                     section->offset_within_address_space,

                     (uint64_t)create.start_addr);

        return -EINVAL;

    }

    trace_vfio_spapr_create_window(create.page_shift,

                                   create.window_size,

                                   create.start_addr);

    *pgsize = pagesize;



    return 0;

}