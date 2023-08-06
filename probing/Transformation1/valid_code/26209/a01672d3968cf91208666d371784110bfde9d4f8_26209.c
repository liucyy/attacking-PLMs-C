static void kvm_set_phys_mem(target_phys_addr_t start_addr, ram_addr_t size,

                             ram_addr_t phys_offset, bool log_dirty)

{

    KVMState *s = kvm_state;

    ram_addr_t flags = phys_offset & ~TARGET_PAGE_MASK;

    KVMSlot *mem, old;

    int err;

    void *ram = NULL;



    

    size = TARGET_PAGE_ALIGN(size);

    start_addr = TARGET_PAGE_ALIGN(start_addr);



    

    phys_offset &= ~IO_MEM_ROM;



    if ((phys_offset & ~TARGET_PAGE_MASK) == IO_MEM_RAM) {

        ram = qemu_safe_ram_ptr(phys_offset);

    }



    while (1) {

        mem = kvm_lookup_overlapping_slot(s, start_addr, start_addr + size);

        if (!mem) {

            break;

        }



        if (flags < IO_MEM_UNASSIGNED && start_addr >= mem->start_addr &&

            (start_addr + size <= mem->start_addr + mem->memory_size) &&

            (ram - start_addr == mem->ram - mem->start_addr)) {

            

            kvm_slot_dirty_pages_log_change(mem, log_dirty);

            return;

        }



        old = *mem;



        

        mem->memory_size = 0;

        err = kvm_set_user_memory_region(s, mem);

        if (err) {

            fprintf(stderr, "%s: error unregistering overlapping slot: %s\n",

                    __func__, strerror(-err));

            abort();

        }



        

        if (s->broken_set_mem_region &&

            old.start_addr == start_addr && old.memory_size < size &&

            flags < IO_MEM_UNASSIGNED) {

            mem = kvm_alloc_slot(s);

            mem->memory_size = old.memory_size;

            mem->start_addr = old.start_addr;

            mem->ram = old.ram;

            mem->flags = kvm_mem_flags(s, log_dirty);



            err = kvm_set_user_memory_region(s, mem);

            if (err) {

                fprintf(stderr, "%s: error updating slot: %s\n", __func__,

                        strerror(-err));

                abort();

            }



            start_addr += old.memory_size;

            phys_offset += old.memory_size;

            ram += old.memory_size;

            size -= old.memory_size;

            continue;

        }



        

        if (old.start_addr < start_addr) {

            mem = kvm_alloc_slot(s);

            mem->memory_size = start_addr - old.start_addr;

            mem->start_addr = old.start_addr;

            mem->ram = old.ram;

            mem->flags =  kvm_mem_flags(s, log_dirty);



            err = kvm_set_user_memory_region(s, mem);

            if (err) {

                fprintf(stderr, "%s: error registering prefix slot: %s\n",

                        __func__, strerror(-err));

#ifdef TARGET_PPC

                fprintf(stderr, "%s: This is probably because your kernel's " \

                                "PAGE_SIZE is too big. Please try to use 4k " \

                                "PAGE_SIZE!\n", __func__);

#endif

                abort();

            }

        }



        

        if (old.start_addr + old.memory_size > start_addr + size) {

            ram_addr_t size_delta;



            mem = kvm_alloc_slot(s);

            mem->start_addr = start_addr + size;

            size_delta = mem->start_addr - old.start_addr;

            mem->memory_size = old.memory_size - size_delta;

            mem->ram = old.ram + size_delta;

            mem->flags = kvm_mem_flags(s, log_dirty);



            err = kvm_set_user_memory_region(s, mem);

            if (err) {

                fprintf(stderr, "%s: error registering suffix slot: %s\n",

                        __func__, strerror(-err));

                abort();

            }

        }

    }



    

    if (!size) {

        return;

    }

    

    if (flags >= IO_MEM_UNASSIGNED) {

        return;

    }

    mem = kvm_alloc_slot(s);

    mem->memory_size = size;

    mem->start_addr = start_addr;

    mem->ram = ram;

    mem->flags = kvm_mem_flags(s, log_dirty);



    err = kvm_set_user_memory_region(s, mem);

    if (err) {

        fprintf(stderr, "%s: error registering slot: %s\n", __func__,

                strerror(-err));

        abort();

    }

}