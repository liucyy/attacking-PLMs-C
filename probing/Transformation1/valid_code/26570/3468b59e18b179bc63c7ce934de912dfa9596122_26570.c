void tcg_region_init(void)

{

    void *buf = tcg_init_ctx.code_gen_buffer;

    void *aligned;

    size_t size = tcg_init_ctx.code_gen_buffer_size;

    size_t page_size = qemu_real_host_page_size;

    size_t region_size;

    size_t n_regions;

    size_t i;



    

    n_regions = 1;



    

    aligned = QEMU_ALIGN_PTR_UP(buf, page_size);

    g_assert(aligned < tcg_init_ctx.code_gen_buffer + size);

    

    region_size = (size - (aligned - buf)) / n_regions;

    region_size = QEMU_ALIGN_DOWN(region_size, page_size);



    

    g_assert(region_size >= 2 * page_size);



    

    qemu_mutex_init(&region.lock);

    region.n = n_regions;

    region.size = region_size - page_size;

    region.stride = region_size;

    region.start = buf;

    region.start_aligned = aligned;

    

    region.end = QEMU_ALIGN_PTR_DOWN(buf + size, page_size);

    

    region.end -= page_size;



    

    for (i = 0; i < region.n; i++) {

        void *start, *end;

        int rc;



        tcg_region_bounds(i, &start, &end);

        rc = qemu_mprotect_none(end, page_size);

        g_assert(!rc);

    }



    

    {

        bool err = tcg_region_initial_alloc__locked(tcg_ctx);



        g_assert(!err);

    }

}