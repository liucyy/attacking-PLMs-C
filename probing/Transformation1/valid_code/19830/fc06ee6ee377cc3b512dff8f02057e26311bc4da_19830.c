static int mm_decode_inter(MmContext * s, int half_horiz, int half_vert)

{

    int data_off = bytestream2_get_le16(&s->gb), y;

    GetByteContext data_ptr;



    if (bytestream2_get_bytes_left(&s->gb) < data_off)

        return AVERROR_INVALIDDATA;



    bytestream2_init(&data_ptr, s->gb.buffer + data_off, bytestream2_get_bytes_left(&s->gb) - data_off);

    while (s->gb.buffer < data_ptr.buffer_start) {

        int i, j;

        int length = bytestream2_get_byte(&s->gb);

        int x = bytestream2_get_byte(&s->gb) + ((length & 0x80) << 1);

        length &= 0x7F;



        if (length==0) {

            y += x;

            continue;

        }



        if (y + half_vert >= s->avctx->height)

            return 0;



        for(i=0; i<length; i++) {

            int replace_array = bytestream2_get_byte(&s->gb);

            for(j=0; j<8; j++) {

                int replace = (replace_array >> (7-j)) & 1;

                if (replace) {

                    int color = bytestream2_get_byte(&data_ptr);

                    s->frame.data[0][y*s->frame.linesize[0] + x] = color;

                    if (half_horiz)

                        s->frame.data[0][y*s->frame.linesize[0] + x + 1] = color;

                    if (half_vert) {

                        s->frame.data[0][(y+1)*s->frame.linesize[0] + x] = color;

                        if (half_horiz)

                            s->frame.data[0][(y+1)*s->frame.linesize[0] + x + 1] = color;

                    }

                }

                x += 1 + half_horiz;

            }

        }



        y += 1 + half_vert;

    }



    return 0;

}