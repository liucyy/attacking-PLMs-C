static int context_init(H264Context *h){

    MpegEncContext * const s = &h->s;



    CHECKED_ALLOCZ(h->top_borders[0], h->s.mb_width * (16+8+8) * sizeof(uint8_t))

    CHECKED_ALLOCZ(h->top_borders[1], h->s.mb_width * (16+8+8) * sizeof(uint8_t))



    

    CHECKED_ALLOCZ(s->allocated_edge_emu_buffer,

                   (s->width+64)*2*21*2); 

    s->edge_emu_buffer= s->allocated_edge_emu_buffer + (s->width+64)*2*21;

    return 0;

fail:

    return -1; 

}