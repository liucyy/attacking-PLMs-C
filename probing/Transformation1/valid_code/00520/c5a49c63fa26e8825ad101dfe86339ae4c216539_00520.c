static bool use_goto_tb(DisasContext *ctx, target_ulong dest)

{

    

    if ((ctx->base.tb->cflags & CF_LAST_IO) || ctx->base.singlestep_enabled) {

        return false;

    }

    return true;

}