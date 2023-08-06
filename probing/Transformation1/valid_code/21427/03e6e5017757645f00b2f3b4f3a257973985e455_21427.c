void cpu_state_reset(CPUMIPSState *env)

{

    if (qemu_loglevel_mask(CPU_LOG_RESET)) {

        qemu_log("CPU Reset (CPU %d)\n", env->cpu_index);

        log_cpu_state(env, 0);

    }



    memset(env, 0, offsetof(CPUMIPSState, breakpoints));

    tlb_flush(env, 1);



    

    env->CP0_PRid = env->cpu_model->CP0_PRid;

    env->CP0_Config0 = env->cpu_model->CP0_Config0;

#ifdef TARGET_WORDS_BIGENDIAN

    env->CP0_Config0 |= (1 << CP0C0_BE);

#endif

    env->CP0_Config1 = env->cpu_model->CP0_Config1;

    env->CP0_Config2 = env->cpu_model->CP0_Config2;

    env->CP0_Config3 = env->cpu_model->CP0_Config3;

    env->CP0_Config6 = env->cpu_model->CP0_Config6;

    env->CP0_Config7 = env->cpu_model->CP0_Config7;

    env->CP0_LLAddr_rw_bitmask = env->cpu_model->CP0_LLAddr_rw_bitmask

                                 << env->cpu_model->CP0_LLAddr_shift;

    env->CP0_LLAddr_shift = env->cpu_model->CP0_LLAddr_shift;

    env->SYNCI_Step = env->cpu_model->SYNCI_Step;

    env->CCRes = env->cpu_model->CCRes;

    env->CP0_Status_rw_bitmask = env->cpu_model->CP0_Status_rw_bitmask;

    env->CP0_TCStatus_rw_bitmask = env->cpu_model->CP0_TCStatus_rw_bitmask;

    env->CP0_SRSCtl = env->cpu_model->CP0_SRSCtl;

    env->current_tc = 0;

    env->SEGBITS = env->cpu_model->SEGBITS;

    env->SEGMask = (target_ulong)((1ULL << env->cpu_model->SEGBITS) - 1);

#if defined(TARGET_MIPS64)

    if (env->cpu_model->insn_flags & ISA_MIPS3) {

        env->SEGMask |= 3ULL << 62;

    }

#endif

    env->PABITS = env->cpu_model->PABITS;

    env->PAMask = (target_ulong)((1ULL << env->cpu_model->PABITS) - 1);

    env->CP0_SRSConf0_rw_bitmask = env->cpu_model->CP0_SRSConf0_rw_bitmask;

    env->CP0_SRSConf0 = env->cpu_model->CP0_SRSConf0;

    env->CP0_SRSConf1_rw_bitmask = env->cpu_model->CP0_SRSConf1_rw_bitmask;

    env->CP0_SRSConf1 = env->cpu_model->CP0_SRSConf1;

    env->CP0_SRSConf2_rw_bitmask = env->cpu_model->CP0_SRSConf2_rw_bitmask;

    env->CP0_SRSConf2 = env->cpu_model->CP0_SRSConf2;

    env->CP0_SRSConf3_rw_bitmask = env->cpu_model->CP0_SRSConf3_rw_bitmask;

    env->CP0_SRSConf3 = env->cpu_model->CP0_SRSConf3;

    env->CP0_SRSConf4_rw_bitmask = env->cpu_model->CP0_SRSConf4_rw_bitmask;

    env->CP0_SRSConf4 = env->cpu_model->CP0_SRSConf4;

    env->active_fpu.fcr0 = env->cpu_model->CP1_fcr0;

    env->insn_flags = env->cpu_model->insn_flags;



#if defined(CONFIG_USER_ONLY)

    env->hflags = MIPS_HFLAG_UM;

    

    env->CP0_HWREna |= 0x0000000F;

    if (env->CP0_Config1 & (1 << CP0C1_FP)) {

        env->hflags |= MIPS_HFLAG_FPU;

    }

#ifdef TARGET_MIPS64

    if (env->active_fpu.fcr0 & (1 << FCR0_F64)) {

        env->hflags |= MIPS_HFLAG_F64;

    }

#endif

#else

    if (env->hflags & MIPS_HFLAG_BMASK) {

        

        env->CP0_ErrorEPC = env->active_tc.PC - 4;

    } else {

        env->CP0_ErrorEPC = env->active_tc.PC;

    }

    env->active_tc.PC = (int32_t)0xBFC00000;

    env->CP0_Random = env->tlb->nb_tlb - 1;

    env->tlb->tlb_in_use = env->tlb->nb_tlb;

    env->CP0_Wired = 0;

    env->CP0_EBase = 0x80000000 | (env->cpu_index & 0x3FF);

    env->CP0_Status = (1 << CP0St_BEV) | (1 << CP0St_ERL);

    

    env->CP0_IntCtl = 0xe0000000;

    {

        int i;



        for (i = 0; i < 7; i++) {

            env->CP0_WatchLo[i] = 0;

            env->CP0_WatchHi[i] = 0x80000000;

        }

        env->CP0_WatchLo[7] = 0;

        env->CP0_WatchHi[7] = 0;

    }

    

    env->CP0_Debug = (1 << CP0DB_CNT) | (0x1 << CP0DB_VER);

    env->hflags = MIPS_HFLAG_CP0;



    if (env->CP0_Config3 & (1 << CP0C3_MT)) {

        int i;



        

        for (i = 0; i < ARRAY_SIZE(env->tcs); i++) {

            env->tcs[i].CP0_TCBind = env->cpu_index << CP0TCBd_CurVPE;

            env->tcs[i].CP0_TCHalt = 1;

        }

        env->active_tc.CP0_TCHalt = 1;

        env->halted = 1;



        if (!env->cpu_index) {

            

            env->mvp->CP0_MVPControl |= (1 << CP0MVPCo_EVP);

            env->CP0_VPEConf0 |= (1 << CP0VPEC0_MVP) | (1 << CP0VPEC0_VPA);



            

            env->halted = 0;

            env->active_tc.CP0_TCHalt = 0;

            env->tcs[0].CP0_TCHalt = 0;

            

            env->active_tc.CP0_TCStatus = (1 << CP0TCSt_A);

            env->tcs[0].CP0_TCStatus = (1 << CP0TCSt_A);

        }

    }

#endif

#if defined(TARGET_MIPS64)

    if (env->cpu_model->insn_flags & ISA_MIPS3) {

        env->hflags |= MIPS_HFLAG_64;

    }

#endif

    env->exception_index = EXCP_NONE;

}