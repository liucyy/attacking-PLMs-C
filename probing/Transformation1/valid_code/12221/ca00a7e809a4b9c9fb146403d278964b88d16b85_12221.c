static int decode_mime_header(AMRWBContext *ctx, const uint8_t *buf)

{

    

    ctx->fr_cur_mode  = buf[0] >> 3 & 0x0F;

    ctx->fr_quality   = (buf[0] & 0x4) != 0x4;



    return 1;

}