static int ipvideo_decode_block_opcode_0x5(IpvideoContext *s)

{

    signed char x, y;



    

    CHECK_STREAM_PTR(2);



    x = *s->stream_ptr++;

    y = *s->stream_ptr++;



    debug_interplay ("    motion bytes = %d, %d\n", x, y);

    return copy_from(s, &s->last_frame, x, y);

}