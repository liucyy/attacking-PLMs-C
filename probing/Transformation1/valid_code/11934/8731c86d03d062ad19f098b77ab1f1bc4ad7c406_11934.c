static int a64_write_packet(struct AVFormatContext *s, AVPacket *pkt)

{

    AVCodecContext *avctx = s->streams[0]->codec;

    A64MuxerContext *c = s->priv_data;

    int i, j;

    int ch_chunksize;

    int lifetime;

    int frame_count;

    int charset_size;

    int frame_size;

    int num_frames;



    

    switch (avctx->codec->id) {

    case CODEC_ID_A64_MULTI:

    case CODEC_ID_A64_MULTI5:

        if(c->interleaved) {

            

            if(avctx->extradata) {

                

                lifetime     = AV_RB32(avctx->extradata + 0);

                frame_count  = AV_RB32(avctx->extradata + 4);

                charset_size = AV_RB32(avctx->extradata + 8);

                frame_size   = AV_RB32(avctx->extradata + 12);



                

            }

            else {

                av_log(avctx, AV_LOG_ERROR, "extradata not set\n");

                return AVERROR(EINVAL);

            }

            ch_chunksize=charset_size/lifetime;

            

            if(pkt->data) num_frames = lifetime;

            else num_frames = c->prev_frame_count;

            for(i = 0; i < num_frames; i++) {

                if(pkt->data) {

                    

                    put_buffer(s->pb, pkt->data + ch_chunksize * i, ch_chunksize);

                }

                else {

                    

                    for(j = 0; j < ch_chunksize; j++) put_byte(s->pb, 0);

                }

                if(c->prev_pkt.data) {

                    

                    put_buffer(s->pb, c->prev_pkt.data + charset_size + frame_size * i, frame_size);

                }

                else {

                    

                    for(j = 0; j < frame_size; j++) put_byte(s->pb, 0);

                }

            }

            

            if(pkt->data) {

                av_new_packet(&c->prev_pkt, pkt->size);

                memcpy(c->prev_pkt.data, pkt->data, pkt->size);

            }

            c->prev_frame_count = frame_count;

            break;

        }

        default:

            

            if(pkt) put_buffer(s->pb, pkt->data, pkt->size);

        break;

    }



    put_flush_packet(s->pb);

    return 0;

}