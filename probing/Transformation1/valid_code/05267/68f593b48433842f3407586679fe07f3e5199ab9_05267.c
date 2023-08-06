static void seek_to_maindata(MPADecodeContext *s, long backstep)

{

    UINT8 *ptr;



    

    ptr = s->gb.buffer + (get_bits_count(&s->gb)>>3);



    

    ptr -= backstep;

    memcpy(ptr, s->inbuf1[s->inbuf_index ^ 1] + 

           BACKSTEP_SIZE + s->old_frame_size - backstep, backstep);

    

    init_get_bits(&s->gb, ptr, s->frame_size + backstep);



    

    s->inbuf_index ^= 1;

    s->inbuf = &s->inbuf1[s->inbuf_index][BACKSTEP_SIZE];

    s->old_frame_size = s->frame_size;

}