static int cdrom_open(BlockDriverState *bs, QDict *options, int flags,

                      Error **errp)

{

    BDRVRawState *s = bs->opaque;

    int ret;



    s->type = FTYPE_CD;



    

    ret = raw_open_common(bs, options, flags, O_NONBLOCK, errp);

    return ret;

}