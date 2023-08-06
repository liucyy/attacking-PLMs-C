static int dirac_decode_frame(AVCodecContext *avctx, void *data, int *data_size, AVPacket *pkt)

{

    DiracContext *s     = avctx->priv_data;

    DiracFrame *picture = data;

    uint8_t *buf        = pkt->data;

    int buf_size        = pkt->size;

    int i, data_unit_size, buf_idx = 0;



    

    for (i = 0; i < MAX_FRAMES; i++)

        if (s->all_frames[i].avframe.data[0] && !s->all_frames[i].avframe.reference) {

            avctx->release_buffer(avctx, &s->all_frames[i].avframe);

            memset(s->all_frames[i].interpolated, 0, sizeof(s->all_frames[i].interpolated));

        }



    s->current_picture = NULL;

    *data_size = 0;



    

    if (buf_size == 0)

        return get_delayed_pic(s, (AVFrame *)data, data_size);



    for (;;) {

        

        for (; buf_idx + DATA_UNIT_HEADER_SIZE < buf_size; buf_idx++) {

            if (buf[buf_idx  ] == 'B' && buf[buf_idx+1] == 'B' &&

                buf[buf_idx+2] == 'C' && buf[buf_idx+3] == 'D')

                break;

        }

        

        if (buf_idx + DATA_UNIT_HEADER_SIZE >= buf_size)

            break;



        data_unit_size = AV_RB32(buf+buf_idx+5);

        if (buf_idx + data_unit_size > buf_size || !data_unit_size) {

            if(buf_idx + data_unit_size > buf_size)

            av_log(s->avctx, AV_LOG_ERROR,

                   "Data unit with size %d is larger than input buffer, discarding\n",

                   data_unit_size);

            buf_idx += 4;

            continue;

        }

        

        if (dirac_decode_data_unit(avctx, buf+buf_idx, data_unit_size))

        {

            av_log(s->avctx, AV_LOG_ERROR,"Error in dirac_decode_data_unit\n");

            return -1;

        }

        buf_idx += data_unit_size;

    }



    if (!s->current_picture)

        return 0;



    if (s->current_picture->avframe.display_picture_number > s->frame_number) {

        DiracFrame *delayed_frame = remove_frame(s->delay_frames, s->frame_number);



        s->current_picture->avframe.reference |= DELAYED_PIC_REF;



        if (add_frame(s->delay_frames, MAX_DELAY, s->current_picture)) {

            int min_num = s->delay_frames[0]->avframe.display_picture_number;

            

            av_log(avctx, AV_LOG_ERROR, "Delay frame overflow\n");

            delayed_frame = s->delay_frames[0];



            for (i = 1; s->delay_frames[i]; i++)

                if (s->delay_frames[i]->avframe.display_picture_number < min_num)

                    min_num = s->delay_frames[i]->avframe.display_picture_number;



            delayed_frame = remove_frame(s->delay_frames, min_num);

            add_frame(s->delay_frames, MAX_DELAY, s->current_picture);

        }



        if (delayed_frame) {

            delayed_frame->avframe.reference ^= DELAYED_PIC_REF;

            *(AVFrame*)data = delayed_frame->avframe;

            *data_size = sizeof(AVFrame);

        }

    } else if (s->current_picture->avframe.display_picture_number == s->frame_number) {

        

        *(AVFrame*)data = s->current_picture->avframe;

        *data_size = sizeof(AVFrame);

    }



    if (*data_size)

        s->frame_number = picture->avframe.display_picture_number + 1;



    return buf_idx;

}