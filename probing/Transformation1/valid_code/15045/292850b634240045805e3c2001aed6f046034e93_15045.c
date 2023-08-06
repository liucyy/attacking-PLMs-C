static int add_shorts_metadata(int count, const char *name,

                               const char *sep, TiffContext *s)

{

    char *ap;

    int i;

    int16_t *sp;



    if (bytestream2_get_bytes_left(&s->gb) < count * sizeof(int16_t))

        return -1;



    sp = av_malloc(count * sizeof(int16_t));

    if (!sp)

        return AVERROR(ENOMEM);



    for (i = 0; i < count; i++)

        sp[i] = tget_short(&s->gb, s->le);

    ap = shorts2str(sp, count, sep);

    av_freep(&sp);

    if (!ap)

        return AVERROR(ENOMEM);

    av_dict_set(&s->picture.metadata, name, ap, AV_DICT_DONT_STRDUP_VAL);

    return 0;

}