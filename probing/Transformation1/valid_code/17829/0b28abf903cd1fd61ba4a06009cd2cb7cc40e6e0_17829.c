static int vble_decode_frame(AVCodecContext *avctx, void *data, int *data_size,
                             AVPacket *avpkt)
{
    VBLEContext *ctx = avctx->priv_data;
    AVFrame *pic = avctx->coded_frame;
    GetBitContext gb;
    const uint8_t *src = avpkt->data;
    int version;
    int offset = 0;
    int width_uv = avctx->width / 2, height_uv = avctx->height / 2;
    pic->reference = 0;
    
    if (pic->data[0])
        avctx->release_buffer(avctx, pic);
    
    if (avctx->get_buffer(avctx, pic) < 0) {
        av_log(avctx, AV_LOG_ERROR, "Could not allocate buffer.\n");
        return AVERROR(ENOMEM);
    
    pic->key_frame = 1;
    pic->pict_type = AV_PICTURE_TYPE_I;
    
    version = AV_RL32(src);
    if (version != 1)
        av_log(avctx, AV_LOG_WARNING, "Unsupported VBLE Version: %d\n", version);
    init_get_bits(&gb, src + 4, (avpkt->size - 4) * 8);
    
    if (vble_unpack(ctx, &gb) < 0) {
        av_log(avctx, AV_LOG_ERROR, "Invalid Code\n");
    
    vble_restore_plane(ctx, &gb, 0, offset, avctx->width, avctx->height);
    
    if (!(ctx->avctx->flags & CODEC_FLAG_GRAY)) {
        offset += avctx->width * avctx->height;
        vble_restore_plane(ctx, &gb, 1, offset, width_uv, height_uv);
        offset += width_uv * height_uv;
        vble_restore_plane(ctx, &gb, 2, offset, width_uv, height_uv);
    *data_size = sizeof(AVFrame);
    *(AVFrame *)data = *pic;
    return avpkt->size;