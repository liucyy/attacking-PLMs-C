void helper_booke206_tlbwe(void)
{
    uint32_t tlbncfg, tlbn;
    ppcmas_tlb_t *tlb;
    uint32_t size_tlb, size_ps;
    switch (env->spr[SPR_BOOKE_MAS0] & MAS0_WQ_MASK) {
    case MAS0_WQ_ALWAYS:
        
        break;
    case MAS0_WQ_COND:
        
        if (0) {
            return;
        break;
    case MAS0_WQ_CLR_RSRV:
        
        return;
    default:
        
        return;
    if (((env->spr[SPR_BOOKE_MAS0] & MAS0_ATSEL) == MAS0_ATSEL_LRAT) &&
         !msr_gs) {
        
        fprintf(stderr, "cpu: don't support LRAT setting yet\n");
        return;
    tlbn = (env->spr[SPR_BOOKE_MAS0] & MAS0_TLBSEL_MASK) >> MAS0_TLBSEL_SHIFT;
    tlbncfg = env->spr[SPR_BOOKE_TLB0CFG + tlbn];
    tlb = booke206_cur_tlb(env);
    if (msr_gs) {
        cpu_abort(env, "missing HV implementation\n");
    tlb->mas7_3 = ((uint64_t)env->spr[SPR_BOOKE_MAS7] << 32) |
                  env->spr[SPR_BOOKE_MAS3];
    tlb->mas1 = env->spr[SPR_BOOKE_MAS1];
    
    tlb->mas2 = env->spr[SPR_BOOKE_MAS2] & 0xffffffff;
    if (!(tlbncfg & TLBnCFG_IPROT)) {
        
        tlb->mas1 &= ~MAS1_IPROT;
    if (booke206_tlb_to_page_size(env, tlb) == TARGET_PAGE_SIZE) {
        tlb_flush_page(env, tlb->mas2 & MAS2_EPN_MASK);
    } else {
        tlb_flush(env, 1);