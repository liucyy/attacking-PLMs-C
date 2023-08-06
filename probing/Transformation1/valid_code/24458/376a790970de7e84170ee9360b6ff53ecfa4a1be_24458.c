void page_set_flags(target_ulong start, target_ulong end, int flags)

{

    PageDesc *p;

    target_ulong addr;



    

    start = start & TARGET_PAGE_MASK;

    end = TARGET_PAGE_ALIGN(end);

    if (flags & PAGE_WRITE)

        flags |= PAGE_WRITE_ORG;

    for(addr = start; addr < end; addr += TARGET_PAGE_SIZE) {

        p = page_find_alloc(addr >> TARGET_PAGE_BITS);

        

        if (!p)

            return;

        

        if (!(p->flags & PAGE_WRITE) &&

            (flags & PAGE_WRITE) &&

            p->first_tb) {

            tb_invalidate_phys_page(addr, 0, NULL);

        }

        p->flags = flags;

    }

}