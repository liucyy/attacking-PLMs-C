static int decode(MimicContext *ctx, int quality, int num_coeffs,

                  int is_iframe)

{

    int y, x, plane, cur_row = 0;



    for(plane = 0; plane < 3; plane++) {

        const int is_chroma = !!plane;

        const int qscale = av_clip(10000-quality,is_chroma?1000:2000,10000)<<2;

        const int stride = ctx->flipped_ptrs[ctx->cur_index].linesize[plane];

        const uint8_t *src = ctx->flipped_ptrs[ctx->prev_index].data[plane];

        uint8_t       *dst = ctx->flipped_ptrs[ctx->cur_index ].data[plane];



        for(y = 0; y < ctx->num_vblocks[plane]; y++) {

            for(x = 0; x < ctx->num_hblocks[plane]; x++) {



                

                if(is_iframe || get_bits1(&ctx->gb) == is_chroma) {



                    

                    if(is_chroma || is_iframe || !get_bits1(&ctx->gb)) {



                        if(!vlc_decode_block(ctx, num_coeffs, qscale))

                            return 0;

                        ctx->dsp.idct_put(dst, stride, ctx->dct_block);

                    } else {

                        unsigned int backref = get_bits(&ctx->gb, 4);

                        int index = (ctx->cur_index+backref)&15;

                        uint8_t *p = ctx->flipped_ptrs[index].data[0];



                        ff_thread_await_progress(&ctx->buf_ptrs[index], cur_row, 0);

                        if(p) {

                            p += src -

                                ctx->flipped_ptrs[ctx->prev_index].data[plane];

                            ctx->dsp.put_pixels_tab[1][0](dst, p, stride, 8);

                        } else {

                            av_log(ctx->avctx, AV_LOG_ERROR,

                                     "No such backreference! Buggy sample.\n");

                        }

                    }

                } else {

                    ff_thread_await_progress(&ctx->buf_ptrs[ctx->prev_index], cur_row, 0);

                    ctx->dsp.put_pixels_tab[1][0](dst, src, stride, 8);

                }

                src += 8;

                dst += 8;

            }

            src += (stride - ctx->num_hblocks[plane])<<3;

            dst += (stride - ctx->num_hblocks[plane])<<3;



            ff_thread_report_progress(&ctx->buf_ptrs[ctx->cur_index], cur_row++, 0);

        }

    }



    return 1;

}