static int gif_read_extension(GifState *s)

{

    int ext_code, ext_len, gce_flags, gce_transparent_index;



    

    if (bytestream2_get_bytes_left(&s->gb) < 2)

        return AVERROR_INVALIDDATA;



    ext_code = bytestream2_get_byteu(&s->gb);

    ext_len = bytestream2_get_byteu(&s->gb);



    av_dlog(s->avctx, "ext_code=0x%x len=%d\n", ext_code, ext_len);



    switch(ext_code) {

    case GIF_GCE_EXT_LABEL:

        if (ext_len != 4)

            goto discard_ext;



        

        if (bytestream2_get_bytes_left(&s->gb) < 5)

            return AVERROR_INVALIDDATA;



        gce_flags = bytestream2_get_byteu(&s->gb);

        bytestream2_skipu(&s->gb, 2);    

        gce_transparent_index = bytestream2_get_byteu(&s->gb);

        if (gce_flags & 0x01)

            s->transparent_color_index = gce_transparent_index;

        else

            s->transparent_color_index = -1;

        s->gce_disposal = (gce_flags >> 2) & 0x7;



        av_dlog(s->avctx, "gce_flags=%x tcolor=%d disposal=%d\n",

               gce_flags,

               s->transparent_color_index, s->gce_disposal);



        if (s->gce_disposal > 3) {

            s->gce_disposal = GCE_DISPOSAL_NONE;

            av_dlog(s->avctx, "invalid value in gce_disposal (%d). Using default value of 0.\n", ext_len);

        }



        ext_len = bytestream2_get_byteu(&s->gb);

        break;

    }



    

 discard_ext:

    while (ext_len != 0) {

        

        if (bytestream2_get_bytes_left(&s->gb) < ext_len + 1)

            return AVERROR_INVALIDDATA;



        bytestream2_skipu(&s->gb, ext_len);

        ext_len = bytestream2_get_byteu(&s->gb);



        av_dlog(s->avctx, "ext_len1=%d\n", ext_len);

    }

    return 0;

}