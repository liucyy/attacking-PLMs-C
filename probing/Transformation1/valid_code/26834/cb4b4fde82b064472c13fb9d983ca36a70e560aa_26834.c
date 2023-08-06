static void vhost_dev_unassign_memory(struct vhost_dev *dev,

                                      uint64_t start_addr,

                                      uint64_t size)

{

    int from, to, n = dev->mem->nregions;

    

    int overlap_start = 0, overlap_end = 0, overlap_middle = 0, split = 0;



    for (from = 0, to = 0; from < n; ++from, ++to) {

        struct vhost_memory_region *reg = dev->mem->regions + to;

        uint64_t reglast;

        uint64_t memlast;

        uint64_t change;



        

        if (to != from) {

            memcpy(reg, dev->mem->regions + from, sizeof *reg);

        }



        

        if (!ranges_overlap(reg->guest_phys_addr, reg->memory_size,

                            start_addr, size)) {

            continue;

        }



        

        assert(!split);



        reglast = range_get_last(reg->guest_phys_addr, reg->memory_size);

        memlast = range_get_last(start_addr, size);



        

        if (start_addr <= reg->guest_phys_addr && memlast >= reglast) {

            --dev->mem->nregions;

            --to;

            assert(to >= 0);

            ++overlap_middle;

            continue;

        }



        

        if (memlast >= reglast) {

            reg->memory_size = start_addr - reg->guest_phys_addr;

            assert(reg->memory_size);

            assert(!overlap_end);

            ++overlap_end;

            continue;

        }



        

        if (start_addr <= reg->guest_phys_addr) {

            change = memlast + 1 - reg->guest_phys_addr;

            reg->memory_size -= change;

            reg->guest_phys_addr += change;

            reg->userspace_addr += change;

            assert(reg->memory_size);

            assert(!overlap_start);

            ++overlap_start;

            continue;

        }



        

        assert(!overlap_start);

        assert(!overlap_end);

        assert(!overlap_middle);

        

        memcpy(dev->mem->regions + n, reg, sizeof *reg);

        reg->memory_size = start_addr - reg->guest_phys_addr;

        assert(reg->memory_size);

        change = memlast + 1 - reg->guest_phys_addr;

        reg = dev->mem->regions + n;

        reg->memory_size -= change;

        assert(reg->memory_size);

        reg->guest_phys_addr += change;

        reg->userspace_addr += change;

        

        assert(dev->mem->nregions == n);

        ++dev->mem->nregions;

        ++split;

    }

}