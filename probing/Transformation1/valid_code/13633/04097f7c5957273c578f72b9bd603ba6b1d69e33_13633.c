static void vhost_client_set_memory(CPUPhysMemoryClient *client,

                                    target_phys_addr_t start_addr,

                                    ram_addr_t size,

                                    ram_addr_t phys_offset,

                                    bool log_dirty)

{

    struct vhost_dev *dev = container_of(client, struct vhost_dev, client);

    ram_addr_t flags = phys_offset & ~TARGET_PAGE_MASK;

    int s = offsetof(struct vhost_memory, regions) +

        (dev->mem->nregions + 1) * sizeof dev->mem->regions[0];

    uint64_t log_size;

    int r;



    dev->mem = g_realloc(dev->mem, s);



    if (log_dirty) {

        flags = IO_MEM_UNASSIGNED;

    }



    assert(size);



    

    if (flags == IO_MEM_RAM) {

        if (!vhost_dev_cmp_memory(dev, start_addr, size,

                                  (uintptr_t)qemu_get_ram_ptr(phys_offset))) {

            

            return;

        }

    } else {

        if (!vhost_dev_find_reg(dev, start_addr, size)) {

            

            return;

        }

    }



    vhost_dev_unassign_memory(dev, start_addr, size);

    if (flags == IO_MEM_RAM) {

        

        vhost_dev_assign_memory(dev, start_addr, size,

                                (uintptr_t)qemu_get_ram_ptr(phys_offset));

    } else {

        

        vhost_dev_unassign_memory(dev, start_addr, size);

    }



    if (!dev->started) {

        return;

    }



    if (dev->started) {

        r = vhost_verify_ring_mappings(dev, start_addr, size);

        assert(r >= 0);

    }



    if (!dev->log_enabled) {

        r = ioctl(dev->control, VHOST_SET_MEM_TABLE, dev->mem);

        assert(r >= 0);

        return;

    }

    log_size = vhost_get_log_size(dev);

    

#define VHOST_LOG_BUFFER (0x1000 / sizeof *dev->log)

    

    if (dev->log_size < log_size) {

        vhost_dev_log_resize(dev, log_size + VHOST_LOG_BUFFER);

    }

    r = ioctl(dev->control, VHOST_SET_MEM_TABLE, dev->mem);

    assert(r >= 0);

    

    if (dev->log_size > log_size + VHOST_LOG_BUFFER) {

        vhost_dev_log_resize(dev, log_size);

    }

}