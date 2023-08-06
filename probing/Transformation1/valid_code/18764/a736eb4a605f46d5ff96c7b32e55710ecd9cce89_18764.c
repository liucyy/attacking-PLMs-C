static void latm_write_frame_header(AVFormatContext *s, PutBitContext *bs)

{

    LATMContext *ctx = s->priv_data;

    AVCodecContext *avctx = s->streams[0]->codec;

    GetBitContext gb;

    int header_size;



    

    put_bits(bs, 1, !!ctx->counter);



    if (!ctx->counter) {

        init_get_bits(&gb, avctx->extradata, avctx->extradata_size * 8);



        

        put_bits(bs, 1, 0); 

        put_bits(bs, 1, 1); 

        put_bits(bs, 6, 0); 

        put_bits(bs, 4, 0); 

        put_bits(bs, 3, 0); 



        

        if (ctx->object_type == AOT_ALS) {

            header_size = avctx->extradata_size-(ctx->off + 7) >> 3;

            avpriv_copy_bits(bs, &avctx->extradata[ctx->off], header_size);

        } else {

            avpriv_copy_bits(bs, avctx->extradata, ctx->off + 3);



            if (!ctx->channel_conf) {

                avpriv_copy_pce_data(bs, &gb);

            }

        }



        put_bits(bs, 3, 0); 

        put_bits(bs, 8, 0xff); 



        put_bits(bs, 1, 0); 

        put_bits(bs, 1, 0); 

    }



    ctx->counter++;

    ctx->counter %= ctx->mod;

}