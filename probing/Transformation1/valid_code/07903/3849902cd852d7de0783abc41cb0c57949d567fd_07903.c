static inline void gen_intermediate_code_internal(CPUState *env,
                                                  TranslationBlock *tb,
                                                  int search_pc)
{
    DisasContext dc1, *dc = &dc1;
    CPUBreakpoint *bp;
    uint16_t *gen_opc_end;
    int j, lj;
    target_ulong pc_start;
    uint32_t next_page_start;
    int num_insns;
    int max_insns;
    
    pc_start = tb->pc;
    dc->tb = tb;
    gen_opc_end = gen_opc_buf + OPC_MAX_SIZE;
    dc->is_jmp = DISAS_NEXT;
    dc->pc = pc_start;
    dc->singlestep_enabled = env->singlestep_enabled;
    dc->condjmp = 0;
    dc->thumb = ARM_TBFLAG_THUMB(tb->flags);
    dc->condexec_mask = (ARM_TBFLAG_CONDEXEC(tb->flags) & 0xf) << 1;
    dc->condexec_cond = ARM_TBFLAG_CONDEXEC(tb->flags) >> 4;
#if !defined(CONFIG_USER_ONLY)
    dc->user = (ARM_TBFLAG_PRIV(tb->flags) == 0);
#endif
    dc->vfp_enabled = ARM_TBFLAG_VFPEN(tb->flags);
    dc->vec_len = ARM_TBFLAG_VECLEN(tb->flags);
    dc->vec_stride = ARM_TBFLAG_VECSTRIDE(tb->flags);
    cpu_F0s = tcg_temp_new_i32();
    cpu_F1s = tcg_temp_new_i32();
    cpu_F0d = tcg_temp_new_i64();
    cpu_F1d = tcg_temp_new_i64();
    cpu_V0 = cpu_F0d;
    cpu_V1 = cpu_F1d;
    
    cpu_M0 = tcg_temp_new_i64();
    next_page_start = (pc_start & TARGET_PAGE_MASK) + TARGET_PAGE_SIZE;
    lj = -1;
    num_insns = 0;
    max_insns = tb->cflags & CF_COUNT_MASK;
    if (max_insns == 0)
        max_insns = CF_COUNT_MASK;
    gen_icount_start();
    tcg_clear_temp_count();
    
    
    if (dc->condexec_mask || dc->condexec_cond)
      {
        TCGv tmp = tcg_temp_new_i32();
        tcg_gen_movi_i32(tmp, 0);
        store_cpu_field(tmp, condexec_bits);
    do {
#ifdef CONFIG_USER_ONLY
        
        if (dc->pc >= 0xffff0000) {
            
            gen_exception(EXCP_KERNEL_TRAP);
            dc->is_jmp = DISAS_UPDATE;
            break;
#else
        if (dc->pc >= 0xfffffff0 && IS_M(env)) {
            
            gen_exception(EXCP_EXCEPTION_EXIT);
            dc->is_jmp = DISAS_UPDATE;
            break;
#endif
        if (unlikely(!QTAILQ_EMPTY(&env->breakpoints))) {
            QTAILQ_FOREACH(bp, &env->breakpoints, entry) {
                if (bp->pc == dc->pc) {
                    gen_exception_insn(dc, 0, EXCP_DEBUG);
                    
                    dc->pc += 2;
                    goto done_generating;
                    break;
        if (search_pc) {
            j = gen_opc_ptr - gen_opc_buf;
            if (lj < j) {
                lj++;
                while (lj < j)
                    gen_opc_instr_start[lj++] = 0;
            gen_opc_pc[lj] = dc->pc;
            gen_opc_condexec_bits[lj] = (dc->condexec_cond << 4) | (dc->condexec_mask >> 1);
            gen_opc_instr_start[lj] = 1;
            gen_opc_icount[lj] = num_insns;
        if (num_insns + 1 == max_insns && (tb->cflags & CF_LAST_IO))
            gen_io_start();
        if (unlikely(qemu_loglevel_mask(CPU_LOG_TB_OP))) {
            tcg_gen_debug_insn_start(dc->pc);
        if (dc->thumb) {
            disas_thumb_insn(env, dc);
            if (dc->condexec_mask) {
                dc->condexec_cond = (dc->condexec_cond & 0xe)
                                   | ((dc->condexec_mask >> 4) & 1);
                dc->condexec_mask = (dc->condexec_mask << 1) & 0x1f;
                if (dc->condexec_mask == 0) {
                    dc->condexec_cond = 0;
        } else {
            disas_arm_insn(env, dc);
        if (dc->condjmp && !dc->is_jmp) {
            gen_set_label(dc->condlabel);
            dc->condjmp = 0;
        
        num_insns ++;
    } while (!dc->is_jmp && gen_opc_ptr < gen_opc_end &&
             !env->singlestep_enabled &&
             !singlestep &&
             dc->pc < next_page_start &&
             num_insns < max_insns);
    if (tb->cflags & CF_LAST_IO) {
        if (dc->condjmp) {
            
            cpu_abort(env, "IO on conditional branch instruction");
        gen_io_end();
    
    if (unlikely(env->singlestep_enabled)) {
        
        if (dc->condjmp) {
            gen_set_condexec(dc);
            if (dc->is_jmp == DISAS_SWI) {
                gen_exception(EXCP_SWI);
            } else {
                gen_exception(EXCP_DEBUG);
            gen_set_label(dc->condlabel);
        if (dc->condjmp || !dc->is_jmp) {
            gen_set_pc_im(dc->pc);
            dc->condjmp = 0;
        gen_set_condexec(dc);
        if (dc->is_jmp == DISAS_SWI && !dc->condjmp) {
            gen_exception(EXCP_SWI);
        } else {
            
            gen_exception(EXCP_DEBUG);
    } else {
        
        gen_set_condexec(dc);
        switch(dc->is_jmp) {
        case DISAS_NEXT:
            gen_goto_tb(dc, 1, dc->pc);
            break;
        default:
        case DISAS_JUMP:
        case DISAS_UPDATE:
            
            tcg_gen_exit_tb(0);
            break;
        case DISAS_TB_JUMP:
            
            break;
        case DISAS_WFI:
            gen_helper_wfi();
            break;
        case DISAS_SWI:
            gen_exception(EXCP_SWI);
            break;
        if (dc->condjmp) {
            gen_set_label(dc->condlabel);
            gen_set_condexec(dc);
            gen_goto_tb(dc, 1, dc->pc);
            dc->condjmp = 0;
done_generating:
    gen_icount_end(tb, num_insns);
    *gen_opc_ptr = INDEX_op_end;
#ifdef DEBUG_DISAS
    if (qemu_loglevel_mask(CPU_LOG_TB_IN_ASM)) {
        qemu_log("----------------\n");
        qemu_log("IN: %s\n", lookup_symbol(pc_start));
        log_target_disas(pc_start, dc->pc - pc_start, dc->thumb);
        qemu_log("\n");
#endif
    if (search_pc) {
        j = gen_opc_ptr - gen_opc_buf;
        lj++;
        while (lj <= j)
            gen_opc_instr_start[lj++] = 0;
    } else {
        tb->size = dc->pc - pc_start;
        tb->icount = num_insns;