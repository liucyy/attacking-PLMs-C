void helper_icbi(target_ulong addr)

{

    addr &= ~(env->dcache_line_size - 1);

    

    ldl(addr);

    tb_invalidate_page_range(addr, addr + env->icache_line_size);

}