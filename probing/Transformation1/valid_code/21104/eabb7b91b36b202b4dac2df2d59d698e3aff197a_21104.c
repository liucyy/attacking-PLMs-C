static void tcg_reg_alloc_bb_end(TCGContext *s, TCGRegSet allocated_regs)

{

    int i;



    for (i = s->nb_globals; i < s->nb_temps; i++) {

        TCGTemp *ts = &s->temps[i];

        if (ts->temp_local) {

            temp_save(s, ts, allocated_regs);

        } else {

#ifdef USE_LIVENESS_ANALYSIS

            

            if (!ts->indirect_base) {

                

                assert(ts->val_type == TEMP_VAL_DEAD);

                continue;

            }

#endif

            temp_dead(s, ts);

        }

    }



    save_globals(s, allocated_regs);

}