static int bethsoftvid_decode_frame(AVCodecContext *avctx,

                              void *data, int *data_size,

                              AVPacket *avpkt)

{

    const uint8_t *buf = avpkt->data;

    int buf_size = avpkt->size;

    BethsoftvidContext * vid = avctx->priv_data;

    char block_type;

    uint8_t * dst;

    uint8_t * frame_end;

    int remaining = avctx->width;          

    const int wrap_to_next_line = vid->frame.linesize[0] - avctx->width;

    int code;

    int yoffset;



    if (avctx->reget_buffer(avctx, &vid->frame)) {

        av_log(avctx, AV_LOG_ERROR, "reget_buffer() failed\n");

        return -1;

    }

    dst = vid->frame.data[0];

    frame_end = vid->frame.data[0] + vid->frame.linesize[0] * avctx->height;



    switch(block_type = *buf++){

        case PALETTE_BLOCK:

            return set_palette(&vid->frame, buf, buf_size);

        case VIDEO_YOFF_P_FRAME:

            yoffset = bytestream_get_le16(&buf);

            if(yoffset >= avctx->height)

                return -1;

            dst += vid->frame.linesize[0] * yoffset;

    }



    

    while((code = *buf++)){

        int length = code & 0x7f;



        

        while(length > remaining){

            if(code < 0x80)

                bytestream_get_buffer(&buf, dst, remaining);

            else if(block_type == VIDEO_I_FRAME)

                memset(dst, buf[0], remaining);

            length -= remaining;      

            dst += remaining + wrap_to_next_line;    

            remaining = avctx->width;

            if(dst == frame_end)

                goto end;

        }



        

        if(code < 0x80)

            bytestream_get_buffer(&buf, dst, length);

        else if(block_type == VIDEO_I_FRAME)

            memset(dst, *buf++, length);

        remaining -= length;

        dst += length;

    }

    end:



    *data_size = sizeof(AVFrame);

    *(AVFrame*)data = vid->frame;



    return buf_size;

}