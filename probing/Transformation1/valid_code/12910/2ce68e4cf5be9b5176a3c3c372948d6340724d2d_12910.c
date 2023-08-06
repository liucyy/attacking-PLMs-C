static void vhost_set_memory(MemoryListener *listener,

                             MemoryRegionSection *section,

                             bool add)

{

    struct vhost_dev *dev = container_of(listener, struct vhost_dev,

                                         memory_listener);

    hwaddr start_addr = section->offset_within_address_space;

    ram_addr_t size = int128_get64(section->size);

    bool log_dirty =

        memory_region_get_dirty_log_mask(section->mr) & ~(1 << DIRTY_MEMORY_MIGRATION);

    int s = offsetof(struct vhost_memory, regions) +

        (dev->mem->nregions + 1) * sizeof dev->mem->regions[0];

    void *ram;



    dev->mem = g_realloc(dev->mem, s);



    if (log_dirty) {

        add = false;

    }



    assert(size);



    

    ram = memory_region_get_ram_ptr(section->mr) + section->offset_within_region;

    if (add) {

        if (!vhost_dev_cmp_memory(dev, start_addr, size, (uintptr_t)ram)) {

            

            return;

        }

    } else {

        if (!vhost_dev_find_reg(dev, start_addr, size)) {

            

            return;

        }

    }



    vhost_dev_unassign_memory(dev, start_addr, size);

    if (add) {

        

        vhost_dev_assign_memory(dev, start_addr, size, (uintptr_t)ram);

    } else {

        

        vhost_dev_unassign_memory(dev, start_addr, size);

    }

    dev->mem_changed_start_addr = MIN(dev->mem_changed_start_addr, start_addr);

    dev->mem_changed_end_addr = MAX(dev->mem_changed_end_addr, start_addr + size - 1);

    dev->memory_changed = true;


}