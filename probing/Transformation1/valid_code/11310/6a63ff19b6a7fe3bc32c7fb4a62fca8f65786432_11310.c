static int mov_read_smi(MOVContext *c, ByteIOContext *pb, MOVAtom atom)

{

    AVStream *st = c->fc->streams[c->fc->nb_streams-1];



    if((uint64_t)atom.size > (1<<30))

        return -1;



    

    

    av_free(st->codec->extradata);

    st->codec->extradata = av_mallocz(atom.size + 0x5a + FF_INPUT_BUFFER_PADDING_SIZE);

    if (!st->codec->extradata)

        return AVERROR(ENOMEM);

    st->codec->extradata_size = 0x5a + atom.size;

    memcpy(st->codec->extradata, "SVQ3", 4); 

    get_buffer(pb, st->codec->extradata + 0x5a, atom.size);

    dprintf(c->fc, "Reading SMI %"PRId64"  %s\n", atom.size, st->codec->extradata + 0x5a);

    return 0;

}