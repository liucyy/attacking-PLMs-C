void tb_flush_jmp_cache(CPUState *cpu, target_ulong addr)

{

    unsigned int i;



    

    i = tb_jmp_cache_hash_page(addr - TARGET_PAGE_SIZE);

    memset(&cpu->tb_jmp_cache[i], 0,

           TB_JMP_PAGE_SIZE * sizeof(TranslationBlock *));



    i = tb_jmp_cache_hash_page(addr);

    memset(&cpu->tb_jmp_cache[i], 0,

           TB_JMP_PAGE_SIZE * sizeof(TranslationBlock *));

}