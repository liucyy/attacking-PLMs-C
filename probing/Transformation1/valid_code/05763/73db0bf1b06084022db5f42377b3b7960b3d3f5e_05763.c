int ff_mpv_frame_size_alloc(MpegEncContext *s, int linesize)

{

    int alloc_size = FFALIGN(FFABS(linesize) + 64, 32);



    

    

    

    

    

    FF_ALLOCZ_OR_GOTO(s->avctx, s->edge_emu_buffer, alloc_size * 4 * 24,

                      fail);



    FF_ALLOCZ_OR_GOTO(s->avctx, s->me.scratchpad, alloc_size * 2 * 16 * 2,

                      fail)

    s->me.temp         = s->me.scratchpad;

    s->rd_scratchpad   = s->me.scratchpad;

    s->b_scratchpad    = s->me.scratchpad;

    s->obmc_scratchpad = s->me.scratchpad + 16;



    return 0;

fail:

    av_freep(&s->edge_emu_buffer);

    return AVERROR(ENOMEM);

}