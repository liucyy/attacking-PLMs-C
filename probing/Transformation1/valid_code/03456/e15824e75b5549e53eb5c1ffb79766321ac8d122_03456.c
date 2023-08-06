int ff_h261_handle_packet(AVFormatContext *ctx, PayloadContext *data,

                          AVStream *st, AVPacket *pkt, uint32_t *timestamp,

                          const uint8_t *buf, int len, uint16_t seq, int flags)

{

    int sbit, ebit, gobn, mbap, quant;

    int res;



    



    

    if (data->buf && data->timestamp != *timestamp) {

        h261_free_dyn_buffer(&data->buf);

    }



    

    if (len < 5 ) {

        av_log(ctx, AV_LOG_ERROR, "Too short H.261 RTP packet\n");

        return AVERROR_INVALIDDATA;

    }



    

    sbit  =  (buf[0] >> 5) & 0x07;

    ebit  =  (buf[0] >> 2) & 0x07;

    gobn  =  (buf[1] >> 4) & 0x0f;

    mbap  = ((buf[1] << 1) & 0x1e) | ((buf[1] >> 7) & 0x01);

    quant =  (buf[1] >> 4) & 0x0f;



    

    buf += RTP_H261_PAYLOAD_HEADER_SIZE;

    len -= RTP_H261_PAYLOAD_HEADER_SIZE;



    

    if (!data->buf) {

        

        if (!gobn  && !sbit && !mbap && !quant){

            res = avio_open_dyn_buf(&data->buf);

            if (res < 0)

                return res;

            

            data->timestamp = *timestamp;

        } else {

            

            return AVERROR(EAGAIN);

        }

    }



    

    if (data->endbyte_bits || sbit) {

        if (data->endbyte_bits == sbit) {

            data->endbyte |= buf[0] & (0xff >> sbit);

            data->endbyte_bits = 0;

            buf++;

            len--;

            avio_w8(data->buf, data->endbyte);

        } else {

            

            GetBitContext gb;

            init_get_bits(&gb, buf, len*8 - ebit);

            skip_bits(&gb, sbit);

            if (data->endbyte_bits) {

                data->endbyte |= get_bits(&gb, 8 - data->endbyte_bits);

                avio_w8(data->buf, data->endbyte);

            }

            while (get_bits_left(&gb) >= 8)

                avio_w8(data->buf, get_bits(&gb, 8));

            data->endbyte_bits = get_bits_left(&gb);

            if (data->endbyte_bits)

                data->endbyte = get_bits(&gb, data->endbyte_bits) <<

                                (8 - data->endbyte_bits);

            ebit = 0;

            len = 0;

        }

    }

    if (ebit) {

        if (len > 0)

            avio_write(data->buf, buf, len - 1);

        data->endbyte_bits = 8 - ebit;

        data->endbyte = buf[len - 1] & (0xff << ebit);

    } else {

        avio_write(data->buf, buf, len);

    }



    

    if (!(flags & RTP_FLAG_MARKER))

        return AVERROR(EAGAIN);



    

    if (data->endbyte_bits)

        avio_w8(data->buf, data->endbyte);

    data->endbyte_bits = 0;



    

    res = ff_rtp_finalize_packet(pkt, &data->buf, st->index);

    if (res < 0)

        return res;



    return 0;

}