int ff_h264_decode_sei(H264Context *h)

{

    while (get_bits_left(&h->gb) > 16) {

        int size = 0;

        int type = 0;

        int ret  = 0;

        int last = 0;



        while (get_bits_left(&h->gb) >= 8 &&

               (last = get_bits(&h->gb, 8)) == 255) {

            type += 255;

        }

        type += last;



        last = 0;

        while (get_bits_left(&h->gb) >= 8 &&

               (last = get_bits(&h->gb, 8)) == 255) {

            size += 255;

        }

        size += last;



        if (size > get_bits_left(&h->gb) / 8) {

            av_log(h->avctx, AV_LOG_ERROR, "SEI type %d truncated at %d\n",

                   type, get_bits_left(&h->gb));

            return AVERROR_INVALIDDATA;

        }



        switch (type) {

        case SEI_TYPE_PIC_TIMING: 

            ret = decode_picture_timing(h);

            if (ret < 0)

                return ret;

            break;

        case SEI_TYPE_USER_DATA_UNREGISTERED:

            ret = decode_unregistered_user_data(h, size);

            if (ret < 0)

                return ret;

            break;

        case SEI_TYPE_RECOVERY_POINT:

            ret = decode_recovery_point(h);

            if (ret < 0)

                return ret;

            break;

        case SEI_TYPE_BUFFERING_PERIOD:

            ret = decode_buffering_period(h);

            if (ret < 0)

                return ret;

            break;

        case SEI_TYPE_FRAME_PACKING:

            ret = decode_frame_packing_arrangement(h);

            if (ret < 0)

                return ret;

            break;

        case SEI_TYPE_DISPLAY_ORIENTATION:

            ret = decode_display_orientation(h);

            if (ret < 0)

                return ret;

            break;

        default:

            av_log(h->avctx, AV_LOG_DEBUG, "unknown SEI type %d\n", type);

            skip_bits(&h->gb, 8 * size);

        }



        

        align_get_bits(&h->gb);

    }



    return 0;

}