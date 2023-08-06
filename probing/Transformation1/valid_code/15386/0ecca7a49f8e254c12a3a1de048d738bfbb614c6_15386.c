static int theora_decode_header(AVCodecContext *avctx, GetBitContext gb)
{
    Vp3DecodeContext *s = avctx->priv_data;
    int major, minor, micro;
    major = get_bits(&gb, 8); 
    minor = get_bits(&gb, 8); 
    micro = get_bits(&gb, 8); 
    av_log(avctx, AV_LOG_INFO, "Theora bitstream version %d.%d.%d\n",
	major, minor, micro);
    
    s->theora = (major << 16) | (minor << 8) | micro;
    
    
    if (s->theora < 0x030300)
    {
	s->flipped_image = 1;
        av_log(avctx, AV_LOG_DEBUG, "Old (<alpha3) Theora bitstream, flipped image\n");
    s->width = get_bits(&gb, 16) << 4;
    s->height = get_bits(&gb, 16) << 4;
    skip_bits(&gb, 24); 
    skip_bits(&gb, 24); 
    skip_bits(&gb, 8); 
    skip_bits(&gb, 8); 
    skip_bits(&gb, 32); 
    skip_bits(&gb, 32); 
    skip_bits(&gb, 24); 
    skip_bits(&gb, 24); 
    if (s->theora < 0x030300)
	skip_bits(&gb, 5); 
    skip_bits(&gb, 8); 
    skip_bits(&gb, 24); 
    skip_bits(&gb, 6); 
    if (s->theora >= 0x030300)
    {
	skip_bits(&gb, 5); 
	skip_bits(&gb, 5); 

    avctx->width = s->width;
    avctx->height = s->height;
    vp3_decode_init(avctx);
    return 0;