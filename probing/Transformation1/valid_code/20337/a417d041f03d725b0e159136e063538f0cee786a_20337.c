static void dnxhd_decode_dct_block(DNXHDContext *ctx, DCTELEM *block, int n, int qscale)

{

    int i, j, index, index2;

    int level, component, sign;

    const uint8_t *weigth_matrix;



    if (n&2) {

        component = 1 + (n&1);

        weigth_matrix = ctx->cid_table->chroma_weigth;

    } else {

        component = 0;

        weigth_matrix = ctx->cid_table->luma_weigth;

    }



    ctx->last_dc[component] += dnxhd_decode_dc(ctx);

    block[0] = ctx->last_dc[component];

    

    for (i = 1; ; i++) {

        index = get_vlc2(&ctx->gb, ctx->ac_vlc.table, DNXHD_VLC_BITS, 2);

        

        level = ctx->cid_table->ac_level[index];

        if (!level) { 

            

            return;

        }

        sign = get_sbits(&ctx->gb, 1);



        if (ctx->cid_table->ac_index_flag[index]) {

            level += get_bits(&ctx->gb, ctx->cid_table->index_bits)<<6;

        }



        if (ctx->cid_table->ac_run_flag[index]) {

            index2 = get_vlc2(&ctx->gb, ctx->run_vlc.table, DNXHD_VLC_BITS, 2);

            i += ctx->cid_table->run[index2];

        }



        j = ctx->scantable.permutated[i];

        

        

        level = (2*level+1) * qscale * weigth_matrix[i];

        if (weigth_matrix[i] != 32) 

            level += 32;

        level >>= 6;

        level = (level^sign) - sign;



        if (i > 63) {

            av_log(ctx->avctx, AV_LOG_ERROR, "ac tex damaged %d, %d\n", n, i);

            return;

        }



        

        block[j] = level;

    }

}