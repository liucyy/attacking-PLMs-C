void tb_set_jmp_target1(uintptr_t jmp_addr, uintptr_t addr)

{

    uint32_t *ptr = (uint32_t *)jmp_addr;

    uintptr_t disp = addr - jmp_addr;



    

    assert(disp == (int32_t)disp);



    *ptr = CALL | (uint32_t)disp >> 2;

    flush_icache_range(jmp_addr, jmp_addr + 4);

}