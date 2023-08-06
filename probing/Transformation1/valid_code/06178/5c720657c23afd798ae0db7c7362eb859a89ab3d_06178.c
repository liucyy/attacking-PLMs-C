static int mov_parse_stsd_data(MOVContext *c, AVIOContext *pb,

                                AVStream *st, MOVStreamContext *sc,

                                int size)

{

    if (st->codec->codec_tag == MKTAG('t','m','c','d')) {

        st->codec->extradata_size = size;

        st->codec->extradata = av_malloc(size + FF_INPUT_BUFFER_PADDING_SIZE);

        if (!st->codec->extradata)

            return AVERROR(ENOMEM);

        avio_read(pb, st->codec->extradata, size);

    } else {

        

        avio_skip(pb, size);

    }

    return 0;

}