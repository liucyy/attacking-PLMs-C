static ExitStatus gen_call_pal(DisasContext *ctx, int palcode)

{

    



    

    if (palcode >= 0x80 && palcode < 0xC0) {

        switch (palcode) {

        case 0x86:

            

            

            break;

        case 0x9E:

            

            tcg_gen_mov_i64(cpu_ir[IR_V0], cpu_unique);

            break;

        case 0x9F:

            

            tcg_gen_mov_i64(cpu_unique, cpu_ir[IR_A0]);

            break;

        default:

            palcode &= 0xbf;

            goto do_call_pal;

        }

        return NO_EXIT;

    }



#ifndef CONFIG_USER_ONLY

    

    if (palcode < 0x40 && (ctx->tb->flags & TB_FLAGS_USER_MODE) == 0) {

        switch (palcode) {

        case 0x01:

            

            

            break;

        case 0x02:

            

            

            break;

        case 0x2D:

            

            tcg_gen_st_i64(cpu_ir[IR_A0], cpu_env, offsetof(CPUAlphaState, vptptr));

            break;

        case 0x31:

            

            tcg_gen_mov_i64(cpu_sysval, cpu_ir[IR_A0]);

            break;

        case 0x32:

            

            tcg_gen_mov_i64(cpu_ir[IR_V0], cpu_sysval);

            break;



        case 0x35: {

            

            TCGv tmp;



            

            tcg_gen_ld8u_i64(cpu_ir[IR_V0], cpu_env, offsetof(CPUAlphaState, ps));



            

            tmp = tcg_temp_new();

            tcg_gen_andi_i64(tmp, cpu_ir[IR_A0], PS_INT_MASK);

            tcg_gen_st8_i64(tmp, cpu_env, offsetof(CPUAlphaState, ps));

            tcg_temp_free(tmp);

            break;

        }



        case 0x36:

            

            tcg_gen_ld8u_i64(cpu_ir[IR_V0], cpu_env, offsetof(CPUAlphaState, ps));

            break;

        case 0x38:

            

            tcg_gen_mov_i64(cpu_usp, cpu_ir[IR_A0]);

            break;

        case 0x3A:

            

            tcg_gen_mov_i64(cpu_ir[IR_V0], cpu_usp);

            break;

        case 0x3C:

            

            tcg_gen_ld32s_i64(cpu_ir[IR_V0], cpu_env,

                -offsetof(AlphaCPU, env) + offsetof(CPUState, cpu_index));

            break;



        default:

            palcode &= 0x3f;

            goto do_call_pal;

        }

        return NO_EXIT;

    }

#endif

    return gen_invalid(ctx);



 do_call_pal:

#ifdef CONFIG_USER_ONLY

    return gen_excp(ctx, EXCP_CALL_PAL, palcode);

#else

    {

        TCGv pc = tcg_const_i64(ctx->pc);

        TCGv entry = tcg_const_i64(palcode & 0x80

                                   ? 0x2000 + (palcode - 0x80) * 64

                                   : 0x1000 + palcode * 64);



        gen_helper_call_pal(cpu_env, pc, entry);



        tcg_temp_free(entry);

        tcg_temp_free(pc);



        

        if (!ctx->singlestep_enabled && !(ctx->tb->cflags & CF_LAST_IO)) {

            tcg_gen_goto_tb(0);

            tcg_gen_exit_tb((uintptr_t)ctx->tb);

            return EXIT_GOTO_TB;

        }



        return EXIT_PC_UPDATED;

    }

#endif

}