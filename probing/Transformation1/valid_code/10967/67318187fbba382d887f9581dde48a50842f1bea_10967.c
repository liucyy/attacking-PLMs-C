static int decode_packet(AVCodecContext *avctx, void *data, int *got_frame_ptr,
                         AVPacket* avpkt)
{
    WmallDecodeCtx *s = avctx->priv_data;
    GetBitContext* gb  = &s->pgb;
    const uint8_t* buf = avpkt->data;
    int buf_size       = avpkt->size;
    int num_bits_prev_frame, packet_sequence_number, spliced_packet;
    s->frame->nb_samples = 0;
    if (!buf_size && s->num_saved_bits > get_bits_count(&s->gb)) {
        s->packet_done = 0;
        if (!decode_frame(s))
            s->num_saved_bits = 0;
    } else if (s->packet_done || s->packet_loss) {
        s->packet_done = 0;
        if (!buf_size)
            return 0;
        s->next_packet_start = buf_size - FFMIN(avctx->block_align, buf_size);
        buf_size             = FFMIN(avctx->block_align, buf_size);
        s->buf_bit_size      = buf_size << 3;
        
        init_get_bits(gb, buf, s->buf_bit_size);
        packet_sequence_number = get_bits(gb, 4);
        skip_bits(gb, 1);   
        spliced_packet = get_bits1(gb);
        if (spliced_packet)
            avpriv_request_sample(avctx, "Bitstream splicing");
        
        num_bits_prev_frame = get_bits(gb, s->log2_frame_size);
        
        if (!s->packet_loss &&
            ((s->packet_sequence_number + 1) & 0xF) != packet_sequence_number) {
            av_log(avctx, AV_LOG_ERROR,
                   "Packet loss detected! seq %"PRIx8" vs %x\n",
                   s->packet_sequence_number, packet_sequence_number);
        s->packet_sequence_number = packet_sequence_number;
        if (num_bits_prev_frame > 0) {
            int remaining_packet_bits = s->buf_bit_size - get_bits_count(gb);
            if (num_bits_prev_frame >= remaining_packet_bits) {
                num_bits_prev_frame = remaining_packet_bits;
                s->packet_done = 1;
            
            save_bits(s, gb, num_bits_prev_frame, 1);
            
            if (num_bits_prev_frame < remaining_packet_bits && !s->packet_loss)
                decode_frame(s);
        } else if (s->num_saved_bits - s->frame_offset) {
            ff_dlog(avctx, "ignoring %x previously saved bits\n",
                    s->num_saved_bits - s->frame_offset);
        if (s->packet_loss) {
            
            s->num_saved_bits = 0;
            s->packet_loss    = 0;
            init_put_bits(&s->pb, s->frame_data, s->max_frame_size);
    } else {
        int frame_size;
        s->buf_bit_size = (avpkt->size - s->next_packet_start) << 3;
        init_get_bits(gb, avpkt->data, s->buf_bit_size);
        skip_bits(gb, s->packet_offset);
        if (s->len_prefix && remaining_bits(s, gb) > s->log2_frame_size &&
            (frame_size = show_bits(gb, s->log2_frame_size)) &&
            frame_size <= remaining_bits(s, gb)) {
            save_bits(s, gb, frame_size, 0);
            s->packet_done = !decode_frame(s);
        } else if (!s->len_prefix
                   && s->num_saved_bits > get_bits_count(&s->gb)) {
            
            s->packet_done = !decode_frame(s);
        } else {
            s->packet_done = 1;
    if (s->packet_done && !s->packet_loss &&
        remaining_bits(s, gb) > 0) {
        
        save_bits(s, gb, remaining_bits(s, gb), 0);
    *got_frame_ptr   = s->frame->nb_samples > 0;
    av_frame_move_ref(data, s->frame);
    s->packet_offset = get_bits_count(gb) & 7;
    return (s->packet_loss) ? AVERROR_INVALIDDATA : buf_size ? get_bits_count(gb) >> 3 : 0;