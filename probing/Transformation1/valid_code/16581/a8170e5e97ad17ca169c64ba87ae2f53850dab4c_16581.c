void framebuffer_update_display(

    DisplayState *ds,

    MemoryRegion *address_space,

    target_phys_addr_t base,

    int cols, 

    int rows, 

    int src_width, 

    int dest_row_pitch, 

    int dest_col_pitch, 

    int invalidate, 

    drawfn fn,

    void *opaque,

    int *first_row, 

    int *last_row )

{

    target_phys_addr_t src_len;

    uint8_t *dest;

    uint8_t *src;

    uint8_t *src_base;

    int first, last = 0;

    int dirty;

    int i;

    ram_addr_t addr;

    MemoryRegionSection mem_section;

    MemoryRegion *mem;



    i = *first_row;

    *first_row = -1;

    src_len = src_width * rows;



    mem_section = memory_region_find(address_space, base, src_len);

    if (mem_section.size != src_len || !memory_region_is_ram(mem_section.mr)) {

        return;

    }

    mem = mem_section.mr;

    assert(mem);

    assert(mem_section.offset_within_address_space == base);



    memory_region_sync_dirty_bitmap(mem);

    src_base = cpu_physical_memory_map(base, &src_len, 0);

    

    if (!src_base)

        return;

    if (src_len != src_width * rows) {

        cpu_physical_memory_unmap(src_base, src_len, 0, 0);

        return;

    }

    src = src_base;

    dest = ds_get_data(ds);

    if (dest_col_pitch < 0)

        dest -= dest_col_pitch * (cols - 1);

    if (dest_row_pitch < 0) {

        dest -= dest_row_pitch * (rows - 1);

    }

    first = -1;

    addr = mem_section.offset_within_region;



    addr += i * src_width;

    src += i * src_width;

    dest += i * dest_row_pitch;



    for (; i < rows; i++) {

        dirty = memory_region_get_dirty(mem, addr, src_width,

                                             DIRTY_MEMORY_VGA);

        if (dirty || invalidate) {

            fn(opaque, dest, src, cols, dest_col_pitch);

            if (first == -1)

                first = i;

            last = i;

        }

        addr += src_width;

        src += src_width;

        dest += dest_row_pitch;

    }

    cpu_physical_memory_unmap(src_base, src_len, 0, 0);

    if (first < 0) {

        return;

    }

    memory_region_reset_dirty(mem, mem_section.offset_within_region, src_len,

                              DIRTY_MEMORY_VGA);

    *first_row = first;

    *last_row = last;

}