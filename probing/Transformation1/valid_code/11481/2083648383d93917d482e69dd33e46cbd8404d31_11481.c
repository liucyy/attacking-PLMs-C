static int MP3lame_encode_frame(AVCodecContext *avctx,

                                unsigned char *frame, int buf_size, void *data)

{

    Mp3AudioContext *s = avctx->priv_data;

    int len;

    int lame_result;



    



    if(data){

        if (s->stereo) {

            lame_result = lame_encode_buffer_interleaved(

                s->gfp,

                data,

                avctx->frame_size,

                s->buffer + s->buffer_index,

                BUFFER_SIZE - s->buffer_index

                );

        } else {

            lame_result = lame_encode_buffer(

                s->gfp,

                data,

                data,

                avctx->frame_size,

                s->buffer + s->buffer_index,

                BUFFER_SIZE - s->buffer_index

                );

        }

    }else{

        lame_result= lame_encode_flush(

                s->gfp,

                s->buffer + s->buffer_index,

                BUFFER_SIZE - s->buffer_index

                );

    }



    if(lame_result==-1) {

        

        av_log(avctx, AV_LOG_ERROR, "lame: output buffer too small (buffer index: %d, free bytes: %d)\n", s->buffer_index, BUFFER_SIZE - s->buffer_index);

        return 0;

    }



    s->buffer_index += lame_result;



    if(s->buffer_index<4)

        return 0;



        len= mp3len(s->buffer, NULL, NULL);



        if(len <= s->buffer_index){

            memcpy(frame, s->buffer, len);

            s->buffer_index -= len;



            memmove(s->buffer, s->buffer+len, s->buffer_index);

            



            return len;

        }else

            return 0;

}