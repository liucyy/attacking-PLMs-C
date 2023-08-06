static void read_const_block_data(ALSDecContext *ctx, ALSBlockData *bd)

{

    ALSSpecificConfig *sconf = &ctx->sconf;

    AVCodecContext *avctx    = ctx->avctx;

    GetBitContext *gb        = &ctx->gb;



    *bd->raw_samples = 0;

    *bd->const_block = get_bits1(gb);    

    bd->js_blocks    = get_bits1(gb);



    

    skip_bits(gb, 5);



    if (*bd->const_block) {

        unsigned int const_val_bits = sconf->floating ? 24 : avctx->bits_per_raw_sample;

        *bd->raw_samples = get_sbits_long(gb, const_val_bits);

    }



    

    *bd->const_block = 1;

}