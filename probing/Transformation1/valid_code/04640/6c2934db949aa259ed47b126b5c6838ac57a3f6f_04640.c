void cpu_physical_memory_rw(target_phys_addr_t addr, uint8_t *buf,

                            int len, int is_write)

{

    int l, io_index;

    uint8_t *ptr;

    uint32_t val;

    target_phys_addr_t page;

    unsigned long pd;

    PhysPageDesc *p;



    while (len > 0) {

        page = addr & TARGET_PAGE_MASK;

        l = (page + TARGET_PAGE_SIZE) - addr;

        if (l > len)

            l = len;

        p = phys_page_find(page >> TARGET_PAGE_BITS);

        if (!p) {

            pd = IO_MEM_UNASSIGNED;

        } else {

            pd = p->phys_offset;

        }



        if (is_write) {

            if ((pd & ~TARGET_PAGE_MASK) != IO_MEM_RAM) {

                io_index = (pd >> IO_MEM_SHIFT) & (IO_MEM_NB_ENTRIES - 1);

                if (p)

                    addr = (addr & ~TARGET_PAGE_MASK) + p->region_offset;

                

                if (l >= 4 && ((addr & 3) == 0)) {

                    

                    val = ldl_p(buf);

                    io_mem_write[io_index][2](io_mem_opaque[io_index], addr, val);

                    l = 4;

                } else if (l >= 2 && ((addr & 1) == 0)) {

                    

                    val = lduw_p(buf);

                    io_mem_write[io_index][1](io_mem_opaque[io_index], addr, val);

                    l = 2;

                } else {

                    

                    val = ldub_p(buf);

                    io_mem_write[io_index][0](io_mem_opaque[io_index], addr, val);

                    l = 1;

                }

            } else {

                unsigned long addr1;

                addr1 = (pd & TARGET_PAGE_MASK) + (addr & ~TARGET_PAGE_MASK);

                

                ptr = phys_ram_base + addr1;

                memcpy(ptr, buf, l);

                if (!cpu_physical_memory_is_dirty(addr1)) {

                    

                    tb_invalidate_phys_page_range(addr1, addr1 + l, 0);

                    

                    phys_ram_dirty[addr1 >> TARGET_PAGE_BITS] |=

                        (0xff & ~CODE_DIRTY_FLAG);

                }

            }

        } else {

            if ((pd & ~TARGET_PAGE_MASK) > IO_MEM_ROM &&

                !(pd & IO_MEM_ROMD)) {

                

                io_index = (pd >> IO_MEM_SHIFT) & (IO_MEM_NB_ENTRIES - 1);

                if (p)

                    addr = (addr & ~TARGET_PAGE_MASK) + p->region_offset;

                if (l >= 4 && ((addr & 3) == 0)) {

                    

                    val = io_mem_read[io_index][2](io_mem_opaque[io_index], addr);

                    stl_p(buf, val);

                    l = 4;

                } else if (l >= 2 && ((addr & 1) == 0)) {

                    

                    val = io_mem_read[io_index][1](io_mem_opaque[io_index], addr);

                    stw_p(buf, val);

                    l = 2;

                } else {

                    

                    val = io_mem_read[io_index][0](io_mem_opaque[io_index], addr);

                    stb_p(buf, val);

                    l = 1;

                }

            } else {

                

                ptr = phys_ram_base + (pd & TARGET_PAGE_MASK) +

                    (addr & ~TARGET_PAGE_MASK);

                memcpy(buf, ptr, l);

            }

        }

        len -= l;

        buf += l;

        addr += l;

    }

}