static void tcg_reg_alloc_op(TCGContext *s, 

                             const TCGOpDef *def, TCGOpcode opc,

                             const TCGArg *args, uint16_t dead_args,

                             uint8_t sync_args)

{

    TCGRegSet allocated_regs;

    int i, k, nb_iargs, nb_oargs, reg;

    TCGArg arg;

    const TCGArgConstraint *arg_ct;

    TCGTemp *ts;

    TCGArg new_args[TCG_MAX_OP_ARGS];

    int const_args[TCG_MAX_OP_ARGS];



    nb_oargs = def->nb_oargs;

    nb_iargs = def->nb_iargs;



    

    memcpy(new_args + nb_oargs + nb_iargs, 

           args + nb_oargs + nb_iargs, 

           sizeof(TCGArg) * def->nb_cargs);



     

    tcg_regset_set(allocated_regs, s->reserved_regs);

    for(k = 0; k < nb_iargs; k++) {

        i = def->sorted_args[nb_oargs + k];

        arg = args[i];

        arg_ct = &def->args_ct[i];

        ts = &s->temps[arg];

        if (ts->val_type == TEMP_VAL_MEM) {

            reg = tcg_reg_alloc(s, arg_ct->u.regs, allocated_regs);

            tcg_out_ld(s, ts->type, reg, ts->mem_reg, ts->mem_offset);

            ts->val_type = TEMP_VAL_REG;

            ts->reg = reg;

            ts->mem_coherent = 1;

            s->reg_to_temp[reg] = arg;

        } else if (ts->val_type == TEMP_VAL_CONST) {

            if (tcg_target_const_match(ts->val, ts->type, arg_ct)) {

                

                const_args[i] = 1;

                new_args[i] = ts->val;

                goto iarg_end;

            } else {

                

                reg = tcg_reg_alloc(s, arg_ct->u.regs, allocated_regs);

                tcg_out_movi(s, ts->type, reg, ts->val);

                ts->val_type = TEMP_VAL_REG;

                ts->reg = reg;

                ts->mem_coherent = 0;

                s->reg_to_temp[reg] = arg;

            }

        }

        assert(ts->val_type == TEMP_VAL_REG);

        if (arg_ct->ct & TCG_CT_IALIAS) {

            if (ts->fixed_reg) {

                

                if (arg != args[arg_ct->alias_index])

                    goto allocate_in_reg;

            } else {

                

                if (!IS_DEAD_ARG(i)) {

                    goto allocate_in_reg;

                }

                

                int k2, i2;

                for (k2 = 0 ; k2 < k ; k2++) {

                    i2 = def->sorted_args[nb_oargs + k2];

                    if ((def->args_ct[i2].ct & TCG_CT_IALIAS) &&

                        (new_args[i2] == ts->reg)) {

                        goto allocate_in_reg;

                    }

                }

            }

        }

        reg = ts->reg;

        if (tcg_regset_test_reg(arg_ct->u.regs, reg)) {

            

        } else {

        allocate_in_reg:

            

            reg = tcg_reg_alloc(s, arg_ct->u.regs, allocated_regs);

            tcg_out_mov(s, ts->type, reg, ts->reg);

        }

        new_args[i] = reg;

        const_args[i] = 0;

        tcg_regset_set_reg(allocated_regs, reg);

    iarg_end: ;

    }

    

    

    for (i = nb_oargs; i < nb_oargs + nb_iargs; i++) {

        if (IS_DEAD_ARG(i)) {

            temp_dead(s, args[i]);

        }

    }



    if (def->flags & TCG_OPF_BB_END) {

        tcg_reg_alloc_bb_end(s, allocated_regs);

    } else {

        if (def->flags & TCG_OPF_CALL_CLOBBER) {

             

            for(reg = 0; reg < TCG_TARGET_NB_REGS; reg++) {

                if (tcg_regset_test_reg(tcg_target_call_clobber_regs, reg)) {

                    tcg_reg_free(s, reg);

                }

            }

        }

        if (def->flags & TCG_OPF_SIDE_EFFECTS) {

            

            sync_globals(s, allocated_regs);

        }

        

        

        tcg_regset_set(allocated_regs, s->reserved_regs);

        for(k = 0; k < nb_oargs; k++) {

            i = def->sorted_args[k];

            arg = args[i];

            arg_ct = &def->args_ct[i];

            ts = &s->temps[arg];

            if (arg_ct->ct & TCG_CT_ALIAS) {

                reg = new_args[arg_ct->alias_index];

            } else {

                

                reg = ts->reg;

                if (ts->fixed_reg &&

                    tcg_regset_test_reg(arg_ct->u.regs, reg)) {

                    goto oarg_end;

                }

                reg = tcg_reg_alloc(s, arg_ct->u.regs, allocated_regs);

            }

            tcg_regset_set_reg(allocated_regs, reg);

            

            if (!ts->fixed_reg) {

                if (ts->val_type == TEMP_VAL_REG) {

                    s->reg_to_temp[ts->reg] = -1;

                }

                ts->val_type = TEMP_VAL_REG;

                ts->reg = reg;

                

                ts->mem_coherent = 0;

                s->reg_to_temp[reg] = arg;

            }

        oarg_end:

            new_args[i] = reg;

        }

    }



    

    tcg_out_op(s, opc, new_args, const_args);

    

    

    for(i = 0; i < nb_oargs; i++) {

        ts = &s->temps[args[i]];

        reg = new_args[i];

        if (ts->fixed_reg && ts->reg != reg) {

            tcg_out_mov(s, ts->type, ts->reg, reg);

        }

        if (NEED_SYNC_ARG(i)) {

            tcg_reg_sync(s, reg);

        }

        if (IS_DEAD_ARG(i)) {

            temp_dead(s, args[i]);

        }

    }

}