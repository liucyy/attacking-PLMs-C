static void flush(AVCodecContext *avctx)

{

    WmallDecodeCtx *s    = avctx->priv_data;

    s->packet_loss       = 1;

    s->packet_done       = 0;

    s->num_saved_bits    = 0;

    s->frame_offset      = 0;

    s->next_packet_start = 0;

    s->cdlms[0][0].order = 0;

    s->frame.nb_samples  = 0;

    init_put_bits(&s->pb, s->frame_data, MAX_FRAMESIZE);

}