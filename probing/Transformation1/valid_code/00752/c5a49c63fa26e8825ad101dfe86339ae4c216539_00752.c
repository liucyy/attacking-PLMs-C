void translator_loop(const TranslatorOps *ops, DisasContextBase *db,

                     CPUState *cpu, TranslationBlock *tb)

{

    int max_insns;



    

    db->tb = tb;

    db->pc_first = tb->pc;

    db->pc_next = db->pc_first;

    db->is_jmp = DISAS_NEXT;

    db->num_insns = 0;

    db->singlestep_enabled = cpu->singlestep_enabled;



    

    max_insns = db->tb->cflags & CF_COUNT_MASK;

    if (max_insns == 0) {

        max_insns = CF_COUNT_MASK;

    }

    if (max_insns > TCG_MAX_INSNS) {

        max_insns = TCG_MAX_INSNS;

    }

    if (db->singlestep_enabled || singlestep) {

        max_insns = 1;

    }



    max_insns = ops->init_disas_context(db, cpu, max_insns);

    tcg_debug_assert(db->is_jmp == DISAS_NEXT);  



    

    tcg_clear_temp_count();



    

    gen_tb_start(db->tb);

    ops->tb_start(db, cpu);

    tcg_debug_assert(db->is_jmp == DISAS_NEXT);  



    while (true) {

        db->num_insns++;

        ops->insn_start(db, cpu);

        tcg_debug_assert(db->is_jmp == DISAS_NEXT);  



        

        if (unlikely(!QTAILQ_EMPTY(&cpu->breakpoints))) {

            CPUBreakpoint *bp;

            QTAILQ_FOREACH(bp, &cpu->breakpoints, entry) {

                if (bp->pc == db->pc_next) {

                    if (ops->breakpoint_check(db, cpu, bp)) {

                        break;

                    }

                }

            }

            

            if (db->is_jmp > DISAS_TOO_MANY) {

                break;

            }

        }



        

        if (db->num_insns == max_insns && (db->tb->cflags & CF_LAST_IO)) {

            

            gen_io_start();

            ops->translate_insn(db, cpu);

            gen_io_end();

        } else {

            ops->translate_insn(db, cpu);

        }



        

        if (db->is_jmp != DISAS_NEXT) {

            break;

        }



        

        if (tcg_op_buf_full() || db->num_insns >= max_insns) {

            db->is_jmp = DISAS_TOO_MANY;

            break;

        }

    }



    

    ops->tb_stop(db, cpu);

    gen_tb_end(db->tb, db->num_insns);



    

    db->tb->size = db->pc_next - db->pc_first;

    db->tb->icount = db->num_insns;



#ifdef DEBUG_DISAS

    if (qemu_loglevel_mask(CPU_LOG_TB_IN_ASM)

        && qemu_log_in_addr_range(db->pc_first)) {

        qemu_log_lock();

        qemu_log("----------------\n");

        ops->disas_log(db, cpu);

        qemu_log("\n");

        qemu_log_unlock();

    }

#endif

}