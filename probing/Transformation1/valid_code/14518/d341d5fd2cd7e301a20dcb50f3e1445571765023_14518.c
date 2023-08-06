static int dnxhd_mb_var_thread(AVCodecContext *avctx, void *arg, int jobnr, int threadnr)

{

    DNXHDEncContext *ctx = avctx->priv_data;

    int mb_y = jobnr, mb_x;

    ctx = ctx->thread[threadnr];

    if (ctx->cid_table->bit_depth == 8) {

        uint8_t *pix = ctx->thread[0]->src[0] + ((mb_y<<4) * ctx->m.linesize);

        for (mb_x = 0; mb_x < ctx->m.mb_width; ++mb_x, pix += 16) {

            unsigned mb  = mb_y * ctx->m.mb_width + mb_x;

            int sum = ctx->m.dsp.pix_sum(pix, ctx->m.linesize);

            int varc = (ctx->m.dsp.pix_norm1(pix, ctx->m.linesize) - (((unsigned)(sum*sum))>>8)+128)>>8;

            ctx->mb_cmp[mb].value = varc;

            ctx->mb_cmp[mb].mb = mb;

        }

    } else { 

        int const linesize = ctx->m.linesize >> 1;

        for (mb_x = 0; mb_x < ctx->m.mb_width; ++mb_x) {

            uint16_t *pix = (uint16_t*)ctx->thread[0]->src[0] + ((mb_y << 4) * linesize) + (mb_x << 4);

            unsigned mb  = mb_y * ctx->m.mb_width + mb_x;

            int sum = 0;

            int sqsum = 0;

            int mean, sqmean;

            int i, j;

            

            for (i = 0; i < 16; ++i) {

                for (j = 0; j < 16; ++j) {

                    

                    int const sample = (unsigned)pix[j] >> 6;

                    sum += sample;

                    sqsum += sample * sample;

                    

                }

                pix += linesize;

            }

            mean = sum >> 8; 

            sqmean = sqsum >> 8;

            ctx->mb_cmp[mb].value = sqmean - mean * mean;

            ctx->mb_cmp[mb].mb = mb;

        }

    }

    return 0;

}