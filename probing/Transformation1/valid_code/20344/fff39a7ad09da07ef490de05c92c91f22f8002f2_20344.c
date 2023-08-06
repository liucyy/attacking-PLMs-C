static void v9fs_unlinkat(void *opaque)
{
    int err = 0;
    V9fsString name;
    int32_t dfid, flags;
    size_t offset = 7;
    V9fsPath path;
    V9fsFidState *dfidp;
    V9fsPDU *pdu = opaque;
    v9fs_string_init(&name);
    err = pdu_unmarshal(pdu, offset, "dsd", &dfid, &name, &flags);
    if (err < 0) {
    dfidp = get_fid(pdu, dfid);
    if (dfidp == NULL) {
        err = -EINVAL;
    
    v9fs_path_init(&path);
    err = v9fs_co_name_to_path(pdu, &dfidp->path, name.data, &path);
    if (err < 0) {
        goto out_err;
    err = v9fs_mark_fids_unreclaim(pdu, &path);
    if (err < 0) {
        goto out_err;
    err = v9fs_co_unlinkat(pdu, &dfidp->path, &name, flags);
    if (!err) {
        err = offset;
out_err:
    put_fid(pdu, dfidp);
    v9fs_path_free(&path);
out_nofid:
    pdu_complete(pdu, err);
    v9fs_string_free(&name);