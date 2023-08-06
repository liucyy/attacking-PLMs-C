static int ipvideo_decode_block_opcode_0xB(IpvideoContext *s)

{

    int y;



    

    CHECK_STREAM_PTR(64);



    for (y = 0; y < 8; y++) {

        memcpy(s->pixel_ptr, s->stream_ptr, 8);

        s->stream_ptr += 8;

        s->pixel_ptr  += s->stride;

    }



    

    return 0;

}