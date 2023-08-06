static int kvm_physical_sync_dirty_bitmap(MemoryRegionSection *section)

{

    KVMState *s = kvm_state;

    unsigned long size, allocated_size = 0;

    KVMDirtyLog d;

    KVMSlot *mem;

    int ret = 0;

    hwaddr start_addr = section->offset_within_address_space;

    hwaddr end_addr = start_addr + int128_get64(section->size);



    d.dirty_bitmap = NULL;

    while (start_addr < end_addr) {

        mem = kvm_lookup_overlapping_slot(s, start_addr, end_addr);

        if (mem == NULL) {

            break;

        }



        

        size = ALIGN(((mem->memory_size) >> TARGET_PAGE_BITS),

                      64) / 8;

        if (!d.dirty_bitmap) {

            d.dirty_bitmap = g_malloc(size);

        } else if (size > allocated_size) {

            d.dirty_bitmap = g_realloc(d.dirty_bitmap, size);

        }

        allocated_size = size;

        memset(d.dirty_bitmap, 0, allocated_size);



        d.slot = mem->slot;



        if (kvm_vm_ioctl(s, KVM_GET_DIRTY_LOG, &d) == -1) {

            DPRINTF("ioctl failed %d\n", errno);

            ret = -1;

            break;

        }



        kvm_get_dirty_pages_log_range(section, d.dirty_bitmap);

        start_addr = mem->start_addr + mem->memory_size;

    }

    g_free(d.dirty_bitmap);



    return ret;

}