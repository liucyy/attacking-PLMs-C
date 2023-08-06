static int flac_write_header(struct AVFormatContext *s)

{

    int ret;

    AVCodecContext *codec = s->streams[0]->codec;

    FlacMuxerContext *c   = s->priv_data;



    if (!c->write_header)

        return 0;



    ret = ff_flac_write_header(s->pb, codec, 0);

    if (ret)

        return ret;



    ret = flac_write_block_comment(s->pb, &s->metadata, 0,

                                   codec->flags & CODEC_FLAG_BITEXACT);

    if (ret)

        return ret;



    

    flac_write_block_padding(s->pb, 8192, 1);



    return ret;

}