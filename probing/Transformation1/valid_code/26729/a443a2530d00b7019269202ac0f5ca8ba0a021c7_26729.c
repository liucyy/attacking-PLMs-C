static int tta_read_header(AVFormatContext *s, AVFormatParameters *ap)
{
    TTAContext *c = s->priv_data;
    AVStream *st;
    int i, channels, bps, samplerate, datalen, framelen, start;
    start = url_ftell(&s->pb);
    if (get_le32(&s->pb) != ff_get_fourcc("TTA1"))
        return -1; 
    url_fskip(&s->pb, 2); 
    channels = get_le16(&s->pb);
    bps = get_le16(&s->pb);
    samplerate = get_le32(&s->pb);
    datalen = get_le32(&s->pb);
    url_fskip(&s->pb, 4); 
    framelen = 1.04489795918367346939 * samplerate;
    c->totalframes = datalen / framelen + ((datalen % framelen) ? 1 : 0);
    c->currentframe = 0;
    c->seektable = av_mallocz(sizeof(uint32_t)*c->totalframes);
    if (!c->seektable)
        return AVERROR_NOMEM;
    for (i = 0; i < c->totalframes; i++)
            c->seektable[i] = get_le32(&s->pb);
    url_fskip(&s->pb, 4); 
    st = av_new_stream(s, 0);

    if (!st)
        return AVERROR_NOMEM;
    st->codec->codec_type = CODEC_TYPE_AUDIO;
    st->codec->codec_id = CODEC_ID_TTA;
    st->codec->channels = channels;
    st->codec->sample_rate = samplerate;
    st->codec->bits_per_sample = bps;
    st->codec->extradata_size = url_ftell(&s->pb) - start;
    if(st->codec->extradata_size+FF_INPUT_BUFFER_PADDING_SIZE <= (unsigned)st->codec->extradata_size){
        
        av_log(s, AV_LOG_ERROR, "extradata_size too large\n");
    st->codec->extradata = av_mallocz(st->codec->extradata_size+FF_INPUT_BUFFER_PADDING_SIZE);
    url_fseek(&s->pb, start, SEEK_SET); 
    get_buffer(&s->pb, st->codec->extradata, st->codec->extradata_size);
    return 0;