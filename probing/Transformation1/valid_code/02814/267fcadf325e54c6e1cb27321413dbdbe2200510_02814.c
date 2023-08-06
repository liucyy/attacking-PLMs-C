static int coroutine_fn v9fs_mark_fids_unreclaim(V9fsPDU *pdu, V9fsPath *path)

{

    int err;

    V9fsState *s = pdu->s;

    V9fsFidState *fidp, head_fid;



    head_fid.next = s->fid_list;

    for (fidp = s->fid_list; fidp; fidp = fidp->next) {

        if (fidp->path.size != path->size) {

            continue;

        }

        if (!memcmp(fidp->path.data, path->data, path->size)) {

            

            fidp->flags |= FID_NON_RECLAIMABLE;



            

            err = v9fs_reopen_fid(pdu, fidp);

            if (err < 0) {

                return -1;

            }

            

            if (err == 0) {

                fidp = &head_fid;

            }

        }

    }

    return 0;

}