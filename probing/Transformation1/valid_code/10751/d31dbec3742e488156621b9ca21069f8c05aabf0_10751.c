static int init_duplicate_context(MpegEncContext *s, MpegEncContext *base){

    int i;



    

    CHECKED_ALLOCZ(s->allocated_edge_emu_buffer, (s->width+64)*2*21*2); 

    s->edge_emu_buffer= s->allocated_edge_emu_buffer + (s->width+64)*2*21;



     

    CHECKED_ALLOCZ(s->me.scratchpad,  (s->width+64)*4*16*2*sizeof(uint8_t))

    s->me.temp=         s->me.scratchpad;

    s->rd_scratchpad=   s->me.scratchpad;

    s->b_scratchpad=    s->me.scratchpad;

    s->obmc_scratchpad= s->me.scratchpad + 16;

    if (s->encoding) {

        CHECKED_ALLOCZ(s->me.map      , ME_MAP_SIZE*sizeof(uint32_t))

        CHECKED_ALLOCZ(s->me.score_map, ME_MAP_SIZE*sizeof(uint32_t))

        if(s->avctx->noise_reduction){

            CHECKED_ALLOCZ(s->dct_error_sum, 2 * 64 * sizeof(int))

        }

    }

    CHECKED_ALLOCZ(s->blocks, 64*12*2 * sizeof(DCTELEM))

    s->block= s->blocks[0];



    for(i=0;i<12;i++){

        s->pblocks[i] = &s->block[i];

    }

    return 0;

fail:

    return -1; 

}