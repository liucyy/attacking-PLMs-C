static int mpegts_handle_packet(AVFormatContext *ctx, PayloadContext *data,

                                AVStream *st, AVPacket *pkt, uint32_t *timestamp,

                                const uint8_t *buf, int len, uint16_t seq,

                                int flags)

{

    int ret;



    

    

    

    

    *timestamp = RTP_NOTS_VALUE;



    if (!data->ts)

        return AVERROR(EINVAL);



    if (!buf) {

        if (data->read_buf_index >= data->read_buf_size)

            return AVERROR(EAGAIN);

        ret = ff_mpegts_parse_packet(data->ts, pkt, data->buf + data->read_buf_index,

                                     data->read_buf_size - data->read_buf_index);

        if (ret < 0)

            return AVERROR(EAGAIN);

        data->read_buf_index += ret;

        if (data->read_buf_index < data->read_buf_size)

            return 1;

        else

            return 0;

    }



    ret = ff_mpegts_parse_packet(data->ts, pkt, buf, len);

    

    if (ret < 0)

        return AVERROR(EAGAIN);

    if (ret < len) {

        data->read_buf_size = FFMIN(len - ret, sizeof(data->buf));

        memcpy(data->buf, buf + ret, data->read_buf_size);

        data->read_buf_index = 0;

        return 1;

    }

    return 0;

}