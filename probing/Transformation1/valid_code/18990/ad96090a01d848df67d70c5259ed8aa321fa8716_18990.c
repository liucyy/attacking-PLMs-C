static int ram_save_live(Monitor *mon, QEMUFile *f, int stage, void *opaque)

{

    ram_addr_t addr;

    uint64_t bytes_transferred_last;

    double bwidth = 0;

    uint64_t expected_time = 0;



    if (stage < 0) {

        cpu_physical_memory_set_dirty_tracking(0);

        return 0;

    }



    if (cpu_physical_sync_dirty_bitmap(0, TARGET_PHYS_ADDR_MAX) != 0) {

        qemu_file_set_error(f);

        return 0;

    }



    if (stage == 1) {

        bytes_transferred = 0;



        

        for (addr = 0; addr < last_ram_offset; addr += TARGET_PAGE_SIZE) {

            if (!cpu_physical_memory_get_dirty(addr, MIGRATION_DIRTY_FLAG))

                cpu_physical_memory_set_dirty(addr);

        }



        

        cpu_physical_memory_set_dirty_tracking(1);



        qemu_put_be64(f, last_ram_offset | RAM_SAVE_FLAG_MEM_SIZE);

    }



    bytes_transferred_last = bytes_transferred;

    bwidth = qemu_get_clock_ns(rt_clock);



    while (!qemu_file_rate_limit(f)) {

        int ret;



        ret = ram_save_block(f);

        bytes_transferred += ret * TARGET_PAGE_SIZE;

        if (ret == 0) 

            break;

    }



    bwidth = qemu_get_clock_ns(rt_clock) - bwidth;

    bwidth = (bytes_transferred - bytes_transferred_last) / bwidth;



    

    if (bwidth == 0)

        bwidth = 0.000001;



    

    if (stage == 3) {

        

        while (ram_save_block(f) != 0) {

            bytes_transferred += TARGET_PAGE_SIZE;

        }

        cpu_physical_memory_set_dirty_tracking(0);

    }



    qemu_put_be64(f, RAM_SAVE_FLAG_EOS);



    expected_time = ram_save_remaining() * TARGET_PAGE_SIZE / bwidth;



    return (stage == 2) && (expected_time <= migrate_max_downtime());

}