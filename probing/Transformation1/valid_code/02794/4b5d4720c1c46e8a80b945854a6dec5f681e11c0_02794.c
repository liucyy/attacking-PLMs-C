static int flac_parse(AVCodecParserContext *s, AVCodecContext *avctx,

                      const uint8_t **poutbuf, int *poutbuf_size,

                      const uint8_t *buf, int buf_size)

{

    FLACParseContext *fpc = s->priv_data;

    FLACHeaderMarker *curr;

    int nb_headers;

    int read_size = 0;



    if (s->flags & PARSER_FLAG_COMPLETE_FRAMES) {

        FLACFrameInfo fi;

        if (frame_header_is_valid(avctx, buf, &fi))

            avctx->frame_size = fi.blocksize;

        *poutbuf      = buf;

        *poutbuf_size = buf_size;

        return buf_size;

    }



    fpc->avctx = avctx;

    if (fpc->best_header_valid)

        return get_best_header(fpc, poutbuf, poutbuf_size);



    

    if (fpc->best_header && fpc->best_header->best_child) {

        FLACHeaderMarker *temp;

        FLACHeaderMarker *best_child = fpc->best_header->best_child;



        

        for (curr = fpc->headers; curr != best_child; curr = temp) {

            if (curr != fpc->best_header) {

                av_log(avctx, AV_LOG_DEBUG,

                       "dropping low score %i frame header from offset %i to %i\n",

                       curr->max_score, curr->offset, curr->next->offset);

            }

            temp = curr->next;

            av_freep(&curr->link_penalty);

            av_free(curr);

            fpc->nb_headers_buffered--;

        }

        

        av_fifo_drain(fpc->fifo_buf, best_child->offset);



        

        for (curr = best_child->next; curr; curr = curr->next)

            curr->offset -= best_child->offset;



        fpc->nb_headers_buffered--;

        best_child->offset = 0;

        fpc->headers       = best_child;

        if (fpc->nb_headers_buffered >= FLAC_MIN_HEADERS) {

            fpc->best_header = best_child;

            return get_best_header(fpc, poutbuf, poutbuf_size);

        }

        fpc->best_header   = NULL;

    } else if (fpc->best_header) {

        

        FLACHeaderMarker *temp;



        for (curr = fpc->headers; curr != fpc->best_header; curr = temp) {

            temp = curr->next;

            av_freep(&curr->link_penalty);

            av_free(curr);

        }

        fpc->headers = fpc->best_header->next;

        av_freep(&fpc->best_header->link_penalty);

        av_freep(&fpc->best_header);

    }



    

    if (buf_size || !fpc->end_padded) {

        int start_offset;



        

        if (!buf_size) {

            fpc->end_padded      = 1;

            buf_size = read_size = MAX_FRAME_HEADER_SIZE;

        } else {

            

            int nb_desired = FLAC_MIN_HEADERS - fpc->nb_headers_buffered + 1;

            read_size = FFMIN(buf_size, nb_desired * FLAC_AVG_FRAME_SIZE);

        }



        

        if (av_fifo_realloc2(fpc->fifo_buf,

                             read_size + av_fifo_size(fpc->fifo_buf)) < 0) {

            av_log(avctx, AV_LOG_ERROR,

                   "couldn't reallocate buffer of size %d\n",

                   read_size + av_fifo_size(fpc->fifo_buf));

            goto handle_error;

        }



        if (buf) {

            av_fifo_generic_write(fpc->fifo_buf, (void*) buf, read_size, NULL);

        } else {

            int8_t pad[MAX_FRAME_HEADER_SIZE];

            memset(pad, 0, sizeof(pad));

            av_fifo_generic_write(fpc->fifo_buf, (void*) pad, sizeof(pad), NULL);

        }



        

        start_offset = av_fifo_size(fpc->fifo_buf) -

                       (read_size + (MAX_FRAME_HEADER_SIZE - 1));

        start_offset = FFMAX(0, start_offset);

        nb_headers   = find_new_headers(fpc, start_offset);



        if (nb_headers < 0) {

            av_log(avctx, AV_LOG_ERROR,

                   "find_new_headers couldn't allocate FLAC header\n");

            goto handle_error;

        }



        fpc->nb_headers_buffered = nb_headers;

        

        if (!fpc->end_padded && fpc->nb_headers_buffered < FLAC_MIN_HEADERS)

            goto handle_error;



        

        if (fpc->end_padded || fpc->nb_headers_found)

            score_sequences(fpc);



        

        if (fpc->end_padded) {

            

            fpc->fifo_buf->wptr -= MAX_FRAME_HEADER_SIZE;

            fpc->fifo_buf->wndx -= MAX_FRAME_HEADER_SIZE;

            if (fpc->fifo_buf->wptr < 0) {

                fpc->fifo_buf->wptr += fpc->fifo_buf->end -

                    fpc->fifo_buf->buffer;

            }

            buf_size = read_size = 0;

        }

    }



    curr = fpc->headers;

    for (curr = fpc->headers; curr; curr = curr->next)

        if (!fpc->best_header || curr->max_score > fpc->best_header->max_score)

            fpc->best_header = curr;



    if (fpc->best_header) {

        fpc->best_header_valid = 1;

        if (fpc->best_header->offset > 0) {

            

            av_log(avctx, AV_LOG_DEBUG, "Junk frame till offset %i\n",

                   fpc->best_header->offset);



            

            avctx->frame_size = 0;

            *poutbuf_size     = fpc->best_header->offset;

            *poutbuf          = flac_fifo_read_wrap(fpc, 0, *poutbuf_size,

                                                    &fpc->wrap_buf,

                                                    &fpc->wrap_buf_allocated_size);

            return buf_size ? read_size : (fpc->best_header->offset -

                                           av_fifo_size(fpc->fifo_buf));

        }

        if (!buf_size)

            return get_best_header(fpc, poutbuf, poutbuf_size);

    }



handle_error:

    *poutbuf      = NULL;

    *poutbuf_size = 0;

    return read_size;

}