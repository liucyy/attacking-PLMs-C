static void v9fs_remove(void *opaque)

{

    int32_t fid;

    int err = 0;

    size_t offset = 7;

    V9fsFidState *fidp;

    V9fsPDU *pdu = opaque;



    pdu_unmarshal(pdu, offset, "d", &fid);




    fidp = get_fid(pdu, fid);

    if (fidp == NULL) {

        err = -EINVAL;

        goto out_nofid;

    }

    

    if (!pdu->s->ctx.flags & PATHNAME_FSCONTEXT) {

        err = -EOPNOTSUPP;

        goto out_err;

    }

    

    err = v9fs_mark_fids_unreclaim(pdu, &fidp->path);

    if (err < 0) {

        goto out_err;

    }

    err = v9fs_co_remove(pdu, &fidp->path);

    if (!err) {

        err = offset;

    }

out_err:

    

    clunk_fid(pdu->s, fidp->fid);

    put_fid(pdu, fidp);

out_nofid:

    complete_pdu(pdu->s, pdu, err);

}