static int mov_read_trun(MOVContext *c, ByteIOContext *pb, MOV_atom_t atom)

{

    MOVFragment *frag = &c->fragment;

    AVStream *st = c->fc->streams[frag->track_id-1];

    MOVStreamContext *sc = st->priv_data;

    uint64_t offset;

    int64_t dts;

    int data_offset = 0;

    unsigned entries, first_sample_flags = frag->flags;

    int flags, distance, i;



    if (sc->pseudo_stream_id+1 != frag->stsd_id)

        return 0;

    if (!st->nb_index_entries)

        return -1;

    get_byte(pb); 

    flags = get_be24(pb);

    entries = get_be32(pb);

    dprintf(c->fc, "flags 0x%x entries %d\n", flags, entries);

    if (flags & 0x001) data_offset        = get_be32(pb);

    if (flags & 0x004) first_sample_flags = get_be32(pb);

    if (flags & 0x800) {

        if ((uint64_t)entries+sc->ctts_count >= UINT_MAX/sizeof(*sc->ctts_data))

            return -1;

        sc->ctts_data = av_realloc(sc->ctts_data,

                                   (entries+sc->ctts_count)*sizeof(*sc->ctts_data));

        if (!sc->ctts_data)

            return AVERROR(ENOMEM);

    }

    dts = st->duration;

    offset = frag->base_data_offset + data_offset;

    distance = 0;

    dprintf(c->fc, "first sample flags 0x%x\n", first_sample_flags);

    for (i = 0; i < entries; i++) {

        unsigned sample_size = frag->size;

        int sample_flags = i ? frag->flags : first_sample_flags;

        unsigned sample_duration = frag->duration;

        int keyframe;



        if (flags & 0x100) sample_duration = get_be32(pb);

        if (flags & 0x200) sample_size     = get_be32(pb);

        if (flags & 0x400) sample_flags    = get_be32(pb);

        if (flags & 0x800) {

            sc->ctts_data[sc->ctts_count].count = 1;

            sc->ctts_data[sc->ctts_count].duration = get_be32(pb);

            sc->ctts_count++;

        }

        if ((keyframe = st->codec->codec_type == CODEC_TYPE_AUDIO ||

             (flags & 0x004 && !i && !sample_flags) || sample_flags & 0x2000000))

            distance = 0;

        av_add_index_entry(st, offset, dts, sample_size, distance,

                           keyframe ? AVINDEX_KEYFRAME : 0);

        dprintf(c->fc, "AVIndex stream %d, sample %d, offset %"PRIx64", dts %"PRId64", "

                "size %d, distance %d, keyframe %d\n", st->index, sc->sample_count+i,

                offset, dts, sample_size, distance, keyframe);

        distance++;

        assert(sample_duration % sc->time_rate == 0);

        dts += sample_duration / sc->time_rate;

        offset += sample_size;

    }

    frag->moof_offset = offset;

    sc->sample_count = st->nb_index_entries;

    st->duration = dts;

    return 0;

}