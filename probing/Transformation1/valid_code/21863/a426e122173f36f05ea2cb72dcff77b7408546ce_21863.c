static void kvm_set_phys_mem(target_phys_addr_t start_addr,

			     ram_addr_t size,

			     ram_addr_t phys_offset)

{

    KVMState *s = kvm_state;

    ram_addr_t flags = phys_offset & ~TARGET_PAGE_MASK;

    KVMSlot *mem, old;

    int err;



    

    size = TARGET_PAGE_ALIGN(size);

    start_addr = TARGET_PAGE_ALIGN(start_addr);



    

    phys_offset &= ~IO_MEM_ROM;



    while (1) {

        mem = kvm_lookup_overlapping_slot(s, start_addr, start_addr + size);

        if (!mem) {

            break;

        }



        if (flags < IO_MEM_UNASSIGNED && start_addr >= mem->start_addr &&

            (start_addr + size <= mem->start_addr + mem->memory_size) &&

            (phys_offset - start_addr == mem->phys_offset - mem->start_addr)) {

            

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

            mem->phys_offset = old.phys_offset;

            mem->flags = 0;



            err = kvm_set_user_memory_region(s, mem);

            if (err) {

                fprintf(stderr, "%s: error updating slot: %s\n", __func__,

                        strerror(-err));

                abort();

            }



            start_addr += old.memory_size;

            phys_offset += old.memory_size;

            size -= old.memory_size;

            continue;

        }



        

        if (old.start_addr < start_addr) {

            mem = kvm_alloc_slot(s);

            mem->memory_size = start_addr - old.start_addr;

            mem->start_addr = old.start_addr;

            mem->phys_offset = old.phys_offset;

            mem->flags = 0;



            err = kvm_set_user_memory_region(s, mem);

            if (err) {

                fprintf(stderr, "%s: error registering prefix slot: %s\n",

                        __func__, strerror(-err));

                abort();

            }

        }



        

        if (old.start_addr + old.memory_size > start_addr + size) {

            ram_addr_t size_delta;



            mem = kvm_alloc_slot(s);

            mem->start_addr = start_addr + size;

            size_delta = mem->start_addr - old.start_addr;

            mem->memory_size = old.memory_size - size_delta;

            mem->phys_offset = old.phys_offset + size_delta;

            mem->flags = 0;



            err = kvm_set_user_memory_region(s, mem);

            if (err) {

                fprintf(stderr, "%s: error registering suffix slot: %s\n",

                        __func__, strerror(-err));

                abort();

            }

        }

    }



    

    if (!size)

        return;



    

    if (flags >= IO_MEM_UNASSIGNED)

        return;



    mem = kvm_alloc_slot(s);

    mem->memory_size = size;

    mem->start_addr = start_addr;

    mem->phys_offset = phys_offset;

    mem->flags = 0;



    err = kvm_set_user_memory_region(s, mem);

    if (err) {

        fprintf(stderr, "%s: error registering slot: %s\n", __func__,

                strerror(-err));

        abort();

    }

}