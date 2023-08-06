static int ipvideo_decode_block_opcode_0xA(IpvideoContext *s, AVFrame *frame)
{
    int x, y;
    unsigned char P[8];
    int flags = 0;
    bytestream2_get_buffer(&s->stream_ptr, P, 4);
    
    if (P[0] <= P[1]) {
        
        for (y = 0; y < 16; y++) {
            
            if (!(y & 3)) {
                if (y) bytestream2_get_buffer(&s->stream_ptr, P, 4);
                flags = bytestream2_get_le32(&s->stream_ptr);
            for (x = 0; x < 4; x++, flags >>= 2)
                *s->pixel_ptr++ = P[flags & 0x03];
            s->pixel_ptr += s->stride - 4;
            
            if (y == 7) s->pixel_ptr -= 8 * s->stride - 4;
    } else {
        
        int vert;
        uint64_t flags = bytestream2_get_le64(&s->stream_ptr);
        bytestream2_get_buffer(&s->stream_ptr, P + 4, 4);
        vert = P[4] <= P[5];
        
        for (y = 0; y < 16; y++) {
            for (x = 0; x < 4; x++, flags >>= 2)
                *s->pixel_ptr++ = P[flags & 0x03];
            if (vert) {
                s->pixel_ptr += s->stride - 4;
                
                if (y == 7) s->pixel_ptr -= 8 * s->stride - 4;
            } else if (y & 1) s->pixel_ptr += s->line_inc;
            
            if (y == 7) {
                memcpy(P, P + 4, 4);
                flags = bytestream2_get_le64(&s->stream_ptr);
    
    return 0;