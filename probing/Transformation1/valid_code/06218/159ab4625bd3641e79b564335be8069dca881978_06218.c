int ff_hevc_decode_extradata(const uint8_t *data, int size, HEVCParamSets *ps,

                             int *is_nalff, int *nal_length_size,

                             int err_recognition, void *logctx)

{

    int ret = 0;

    GetByteContext gb;



    bytestream2_init(&gb, data, size);



    if (size > 3 && (data[0] || data[1] || data[2] > 1)) {

        

        int i, j, num_arrays, nal_len_size;



        *is_nalff = 1;



        bytestream2_skip(&gb, 21);

        nal_len_size = (bytestream2_get_byte(&gb) & 3) + 1;

        num_arrays   = bytestream2_get_byte(&gb);



        

        *nal_length_size = 2;



        

        for (i = 0; i < num_arrays; i++) {

            int type = bytestream2_get_byte(&gb) & 0x3f;

            int cnt  = bytestream2_get_be16(&gb);



            for (j = 0; j < cnt; j++) {

                

                int nalsize = bytestream2_peek_be16(&gb) + 2;

                if (bytestream2_get_bytes_left(&gb) < nalsize) {

                    av_log(logctx, AV_LOG_ERROR,

                           "Invalid NAL unit size in extradata.\n");

                    return AVERROR_INVALIDDATA;

                }



                ret = hevc_decode_nal_units(gb.buffer, nalsize, ps, *is_nalff, *nal_length_size, logctx);

                if (ret < 0) {

                    av_log(logctx, AV_LOG_ERROR,

                           "Decoding nal unit %d %d from hvcC failed\n",

                           type, i);

                    return ret;

                }

                bytestream2_skip(&gb, nalsize);

            }

        }



        

        *nal_length_size = nal_len_size;

    } else {

        *is_nalff = 0;

        ret = hevc_decode_nal_units(data, size, ps, *is_nalff, *nal_length_size, logctx);

        if (ret < 0)

            return ret;

    }



    return ret;

}