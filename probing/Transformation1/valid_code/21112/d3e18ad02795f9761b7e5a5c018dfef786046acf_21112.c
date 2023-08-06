static int swf_write_video(AVFormatContext *s, 

                           AVCodecContext *enc, const uint8_t *buf, int size)

{

    SWFContext *swf = s->priv_data;

    ByteIOContext *pb = &s->pb;

    int c = 0;

    int outSize = 0;

    int outSamples = 0;

    

    

    if ( swf->swf_frame_number >= 16000 ) {

        return 0;

    }



    

    if ( enc->codec_type == CODEC_TYPE_VIDEO ) {

        SWFFrame *new_frame = av_malloc(sizeof(SWFFrame));

        new_frame->prev = 0;

        new_frame->next = swf->frame_head;

        new_frame->data = av_malloc(size);

        new_frame->size = size;

        memcpy(new_frame->data,buf,size);

        swf->frame_head = new_frame;

        if ( swf->frame_tail == 0 ) {

            swf->frame_tail = new_frame;

        }

    }

    

    if ( swf->audio_type ) {

        

retry_swf_audio_packet:

        if ( ( swf->audio_size-outSize ) >= 4 ) {

            int mp3FrameSize = 0;

            int mp3SampleRate = 0;

            int mp3IsMono = 0;

            int mp3SamplesPerFrame = 0;

            

            

            uint8_t header[4];

            for (c=0; c<4; c++) {

                header[c] = swf->audio_fifo[(swf->audio_in_pos+outSize+c) % AUDIO_FIFO_SIZE];

            }

            

            if ( swf_mp3_info(header,&mp3FrameSize,&mp3SamplesPerFrame,&mp3SampleRate,&mp3IsMono) ) {

                if ( ( swf->audio_size-outSize ) >= mp3FrameSize ) {

                    outSize += mp3FrameSize;

                    outSamples += mp3SamplesPerFrame;

                    if ( ( swf->sound_samples + outSamples + swf->samples_per_frame ) < swf->video_samples ) {

                        goto retry_swf_audio_packet;

                    }

                }

            } else {

                

                swf->audio_in_pos ++;

                swf->audio_size --;

                swf->audio_in_pos %= AUDIO_FIFO_SIZE;

                goto retry_swf_audio_packet;

            }

        }

        

        

        if ( ( swf->sound_samples + outSamples + swf->samples_per_frame ) < swf->video_samples ) {

            return 0;

        }



        

        if ( enc->codec_type == CODEC_TYPE_VIDEO ) {

            swf->skip_samples = (int)( ( (double)(swf->swf_frame_number) * (double)enc->frame_rate_base * 44100. ) / (double)(enc->frame_rate) );

            swf->skip_samples -=  swf->video_samples;

        }

    }



    

    if (swf->skip_samples <= ( swf->samples_per_frame / 2 ) ) {

        

        if ( swf->frame_tail ) {

            if ( swf->video_type == CODEC_ID_FLV1 ) {

                if ( swf->video_frame_number == 0 ) {

                    

                    put_swf_tag(s, TAG_VIDEOSTREAM);

                    put_le16(pb, VIDEO_ID);

                    put_le16(pb, 15000 ); 

                    put_le16(pb, enc->width);

                    put_le16(pb, enc->height);

                    put_byte(pb, 0);

                    put_byte(pb, SWF_VIDEO_CODEC_FLV1);

                    put_swf_end_tag(s);

                    

                    

                    put_swf_tag(s, TAG_PLACEOBJECT2);

                    put_byte(pb, 0x36);

                    put_le16(pb, 1);

                    put_le16(pb, VIDEO_ID);

                    put_swf_matrix(pb, 1 << FRAC_BITS, 0, 0, 1 << FRAC_BITS, 0, 0);

                    put_le16(pb, swf->video_frame_number );

                    put_byte(pb, 'v');

                    put_byte(pb, 'i');

                    put_byte(pb, 'd');

                    put_byte(pb, 'e');

                    put_byte(pb, 'o');

                    put_byte(pb, 0x00);

                    put_swf_end_tag(s);

                } else {

                    

                    put_swf_tag(s, TAG_PLACEOBJECT2);

                    put_byte(pb, 0x11);

                    put_le16(pb, 1);

                    put_le16(pb, swf->video_frame_number );

                    put_swf_end_tag(s);

                }

    

                

                for (; ( enc->frame_number - swf->video_frame_number ) > 0;) {

                    

                    put_swf_tag(s, TAG_VIDEOFRAME | TAG_LONG);

                    put_le16(pb, VIDEO_ID); 

                    put_le16(pb, swf->video_frame_number++ );

                    put_buffer(pb, swf->frame_tail->data, swf->frame_tail->size);

                    put_swf_end_tag(s);

                }



            } else if ( swf->video_type == CODEC_ID_MJPEG ) {

                if (swf->swf_frame_number > 0) {

                    

                    put_swf_tag(s, TAG_REMOVEOBJECT);

                    put_le16(pb, SHAPE_ID); 

                    put_le16(pb, 1); 

                    put_swf_end_tag(s);

                

                    

                    put_swf_tag(s, TAG_FREECHARACTER);

                    put_le16(pb, BITMAP_ID);

                    put_swf_end_tag(s);

                }

        

                put_swf_tag(s, TAG_JPEG2 | TAG_LONG);

        

                put_le16(pb, BITMAP_ID); 

        

                

                put_byte(pb, 0xff); 

                put_byte(pb, 0xd8);

                put_byte(pb, 0xff);

                put_byte(pb, 0xd9);

                

                put_buffer(pb, swf->frame_tail->data, swf->frame_tail->size);

        

                put_swf_end_tag(s);

        

                

        

                put_swf_tag(s, TAG_PLACEOBJECT);

                put_le16(pb, SHAPE_ID); 

                put_le16(pb, 1); 

                put_swf_matrix(pb, 20 << FRAC_BITS, 0, 0, 20 << FRAC_BITS, 0, 0);

                put_swf_end_tag(s);

            } else {

                

            }

    

            av_free(swf->frame_tail->data);

            swf->frame_tail = swf->frame_tail->prev;

            if ( swf->frame_tail ) {

                if ( swf->frame_tail->next ) {

                    av_free(swf->frame_tail->next);

                }

                swf->frame_tail->next = 0;

            } else {

                swf->frame_head = 0;

            }

            swf->swf_frame_number ++;

        }

    }



    swf->video_samples += swf->samples_per_frame;



    

    if ( outSize > 0 ) {

        put_swf_tag(s, TAG_STREAMBLOCK | TAG_LONG);

        put_le16(pb, outSamples);

        put_le16(pb, 0);

        for (c=0; c<outSize; c++) {

            put_byte(pb,swf->audio_fifo[(swf->audio_in_pos+c) % AUDIO_FIFO_SIZE]);

        }

        put_swf_end_tag(s);

    

        

        swf->sound_samples += outSamples;

        swf->audio_in_pos += outSize;

        swf->audio_size -= outSize;

        swf->audio_in_pos %= AUDIO_FIFO_SIZE;

    }



    

    put_swf_tag(s, TAG_SHOWFRAME);

    put_swf_end_tag(s);

    

    put_flush_packet(&s->pb);

    

    return 0;

}