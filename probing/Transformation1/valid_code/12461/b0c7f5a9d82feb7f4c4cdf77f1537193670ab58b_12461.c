int av_write_frame(AVFormatContext *s, int stream_index, const uint8_t *buf, 

                   int size)

{

    AVStream *st;

    int64_t pts_mask;

    int ret, frame_size;



    st = s->streams[stream_index];

    pts_mask = (1LL << s->pts_wrap_bits) - 1;

    ret = s->oformat->write_packet(s, stream_index, buf, size, 

                                   st->pts.val & pts_mask);

    if (ret < 0)

        return ret;



    

    switch (st->codec.codec_type) {

    case CODEC_TYPE_AUDIO:

        frame_size = get_audio_frame_size(&st->codec, size);



        



        if (frame_size >= 0 && (size || st->pts.num!=st->pts.den>>1 || st->pts.val)) {

            av_frac_add(&st->pts, 

                        (int64_t)s->pts_den * frame_size);

        }

        break;

    case CODEC_TYPE_VIDEO:

        av_frac_add(&st->pts, 

                    (int64_t)s->pts_den * st->codec.frame_rate_base);

        break;

    default:

        break;

    }

    return ret;

}