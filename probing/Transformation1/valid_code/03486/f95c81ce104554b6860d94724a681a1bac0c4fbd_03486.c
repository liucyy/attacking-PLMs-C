static av_cold int mov_text_encode_init(AVCodecContext *avctx)

{

    

    static const uint8_t text_sample_entry[] = {

        0x00, 0x00, 0x00, 0x00, 

        0x01,                   

        0xFF,                   

        0x00, 0x00, 0x00, 0x00, 

        

        0x00, 0x00,             

        0x00, 0x00,             

        0x00, 0x00,             

        0x00, 0x00,             

        

        

        0x00, 0x00,             

        0x00, 0x00,             

        0x00, 0x01,             

        0x00,                   

        0x12,                   

        0xFF, 0xFF, 0xFF, 0xFF, 

        

        

        0x00, 0x00, 0x00, 0x12, 

        'f', 't', 'a', 'b',     

        0x00, 0x01,             

        

        0x00, 0x01,             

        0x05,                   

        'S', 'e', 'r', 'i', 'f',

        

        

    };



    MovTextContext *s = avctx->priv_data;




    avctx->extradata_size = sizeof text_sample_entry;

    avctx->extradata = av_mallocz(avctx->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);

    if (!avctx->extradata)

        return AVERROR(ENOMEM);



    av_bprint_init(&s->buffer, 0, AV_BPRINT_SIZE_UNLIMITED);



    memcpy(avctx->extradata, text_sample_entry, avctx->extradata_size);



    s->ass_ctx = ff_ass_split(avctx->subtitle_header);

    return s->ass_ctx ? 0 : AVERROR_INVALIDDATA;

}