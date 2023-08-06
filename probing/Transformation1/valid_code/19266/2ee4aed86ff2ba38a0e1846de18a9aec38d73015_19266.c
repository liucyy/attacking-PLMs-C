void do_tlbwi (void)

{

    

    mips_tlb_flush_extra (env, MIPS_TLB_NB);



    

    invalidate_tlb(env->CP0_index & (MIPS_TLB_NB - 1), 0);

    fill_tlb(env->CP0_index & (MIPS_TLB_NB - 1));

}