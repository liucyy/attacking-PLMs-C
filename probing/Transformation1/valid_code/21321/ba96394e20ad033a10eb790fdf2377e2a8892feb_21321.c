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

            return gen_excp(ctx, EXCP_CALL_PAL, palcode & 0xbf);

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

            return gen_excp(ctx, EXCP_CALL_PAL, palcode & 0x3f);

        }

        return NO_EXIT;

    }

#endif



    return gen_invalid(ctx);

}