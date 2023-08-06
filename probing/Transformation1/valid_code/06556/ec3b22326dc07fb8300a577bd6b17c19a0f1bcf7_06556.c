static int http_prepare_data(HTTPContext *c)

{

    int i;



    switch(c->state) {

    case HTTPSTATE_SEND_DATA_HEADER:

        memset(&c->fmt_ctx, 0, sizeof(c->fmt_ctx));

        if (c->stream->feed) {

            

            c->fmt_ctx.oformat = c->stream->fmt;

            c->fmt_ctx.nb_streams = c->stream->nb_streams;

            for(i=0;i<c->fmt_ctx.nb_streams;i++) {

                AVStream *st;

                st = av_mallocz(sizeof(AVStream));

                c->fmt_ctx.streams[i] = st;

                if (c->stream->feed == c->stream)

                    memcpy(st, c->stream->streams[i], sizeof(AVStream));

                else

                    memcpy(st, c->stream->feed->streams[c->stream->feed_streams[i]], sizeof(AVStream));



                st->codec.frame_number = 0; 

            }

            c->got_key_frame = 0;

        } else {

            

            c->fmt_ctx.oformat = c->stream->fmt;

            c->fmt_ctx.nb_streams = c->fmt_in->nb_streams;

            for(i=0;i<c->fmt_ctx.nb_streams;i++) {

                AVStream *st;

                st = av_mallocz(sizeof(AVStream));

                c->fmt_ctx.streams[i] = st;

                memcpy(st, c->fmt_in->streams[i], sizeof(AVStream));

                st->codec.frame_number = 0; 

            }

            c->got_key_frame = 0;

        }

        init_put_byte(&c->fmt_ctx.pb, c->pbuffer, PACKET_MAX_SIZE,

                      1, c, NULL, http_write_packet, NULL);

        c->fmt_ctx.pb.is_streamed = 1;

        

        av_write_header(&c->fmt_ctx);

        c->state = HTTPSTATE_SEND_DATA;

        c->last_packet_sent = 0;

        break;

    case HTTPSTATE_SEND_DATA:

        

#if 0

        fifo_total_size = http_fifo_write_count - c->last_http_fifo_write_count;

        if (fifo_total_size >= ((3 * FIFO_MAX_SIZE) / 4)) {

            

            c->rptr = http_fifo.wptr;

            c->got_key_frame = 0;

        }

        

        start_rptr = c->rptr;

        if (fifo_read(&http_fifo, (UINT8 *)&hdr, sizeof(hdr), &c->rptr) < 0)

            return 0;

        payload_size = ntohs(hdr.payload_size);

        payload = av_malloc(payload_size);

        if (fifo_read(&http_fifo, payload, payload_size, &c->rptr) < 0) {

            

            av_free(payload);

            c->rptr = start_rptr;

            return 0;

        }

        

        c->last_http_fifo_write_count = http_fifo_write_count - 

            fifo_size(&http_fifo, c->rptr);

        

        if (c->stream->stream_type != STREAM_TYPE_MASTER) {

            

            ret = 0;

            for(i=0;i<c->fmt_ctx.nb_streams;i++) {

                AVStream *st = c->fmt_ctx.streams[i];

                if (test_header(&hdr, &st->codec)) {

                    

                    if (st->codec.key_frame)

                        c->got_key_frame |= 1 << i;

                    if (c->got_key_frame & (1 << i)) {

                        ret = c->fmt_ctx.format->write_packet(&c->fmt_ctx, i,

                                                                   payload, payload_size);

                    }

                    break;

                }

            }

            if (ret) {

                

                c->state = HTTPSTATE_SEND_DATA_TRAILER;

            }

        } else {

            

            char *q;

            q = c->buffer;

            memcpy(q, &hdr, sizeof(hdr));

            q += sizeof(hdr);

            memcpy(q, payload, payload_size);

            q += payload_size;

            c->buffer_ptr = c->buffer;

            c->buffer_end = q;

        }

        av_free(payload);

#endif

        {

            AVPacket pkt;



            

            if (c->stream->feed) {

                ffm_set_write_index(c->fmt_in, 

                                    c->stream->feed->feed_write_index,

                                    c->stream->feed->feed_size);

            }

            

            if (av_read_packet(c->fmt_in, &pkt) < 0) {

                if (c->stream->feed && c->stream->feed->feed_opened) {

                    

                    c->state = HTTPSTATE_WAIT_FEED;

                    return 1; 

                } else {

                    

                    c->state = HTTPSTATE_SEND_DATA_TRAILER;

                }

            } else {

                

                if (c->stream->feed) {

                    

                    for(i=0;i<c->stream->nb_streams;i++) {

                        if (c->stream->feed_streams[i] == pkt.stream_index) {

                            pkt.stream_index = i;

                            if (pkt.flags & PKT_FLAG_KEY) {

                                c->got_key_frame |= 1 << i;

                            }

                            

                            if (!c->stream->send_on_key || ((c->got_key_frame + 1) >> c->stream->nb_streams)) {

                                goto send_it;

                            }

                        }

                    }

                } else {

                    AVCodecContext *codec;

                send_it:

                    

                    codec = &c->fmt_ctx.streams[pkt.stream_index]->codec;



                    codec->key_frame = ((pkt.flags & PKT_FLAG_KEY) != 0);



#ifdef PJSG

                    if (codec->codec_type == CODEC_TYPE_AUDIO) {

                        codec->frame_size = (codec->sample_rate * pkt.duration + 500000) / 1000000;

                        

                    }

#endif



                    if (av_write_packet(&c->fmt_ctx, &pkt, 0))

                        c->state = HTTPSTATE_SEND_DATA_TRAILER;



                    codec->frame_number++;

                }



                av_free_packet(&pkt);

            }

        }

        break;

    default:

    case HTTPSTATE_SEND_DATA_TRAILER:

        

        if (c->last_packet_sent)

            return -1;

        

        av_write_trailer(&c->fmt_ctx);

        c->last_packet_sent = 1;

        break;

    }

    return 0;

}