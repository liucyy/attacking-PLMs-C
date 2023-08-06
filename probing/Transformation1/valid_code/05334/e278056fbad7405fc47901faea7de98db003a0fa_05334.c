static int theora_decode_init(AVCodecContext *avctx)

{

    Vp3DecodeContext *s = avctx->priv_data;

    GetBitContext gb;

    int ptype;

    uint8_t *p= avctx->extradata;

    int op_bytes, i;



    s->theora = 1;



    if (!avctx->extradata_size)

    {

        av_log(avctx, AV_LOG_ERROR, "Missing extradata!\n");

        return -1;

    }



  for(i=0;i<3;i++) {

    op_bytes = *(p++)<<8;

    op_bytes += *(p++);



    init_get_bits(&gb, p, op_bytes);

    p += op_bytes;



    ptype = get_bits(&gb, 8);

    debug_vp3("Theora headerpacket type: %x\n", ptype);



     if (!(ptype & 0x80))

     {

        av_log(avctx, AV_LOG_ERROR, "Invalid extradata!\n");

        return -1;

     }



    

    skip_bits(&gb, 6*8); 



    switch(ptype)

    {

        case 0x80:

            theora_decode_header(avctx, gb);

                break;

        case 0x81:





            break;

        case 0x82:

            theora_decode_tables(avctx, gb);

            break;

        default:

            av_log(avctx, AV_LOG_ERROR, "Unknown Theora config packet: %d\n", ptype&~0x80);

            break;

    }

  }



    vp3_decode_init(avctx);

    return 0;

}