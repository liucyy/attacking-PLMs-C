int qcow2_snapshot_delete(BlockDriverState *bs,

                          const char *snapshot_id,

                          const char *name,

                          Error **errp)

{

    BDRVQcowState *s = bs->opaque;

    QCowSnapshot sn;

    int snapshot_index, ret;



    

    snapshot_index = find_snapshot_by_id_and_name(bs, snapshot_id, name);

    if (snapshot_index < 0) {

        error_setg(errp, "Can't find the snapshot");

        return -ENOENT;

    }

    sn = s->snapshots[snapshot_index];



    

    memmove(s->snapshots + snapshot_index,

            s->snapshots + snapshot_index + 1,

            (s->nb_snapshots - snapshot_index - 1) * sizeof(sn));

    s->nb_snapshots--;

    ret = qcow2_write_snapshots(bs);

    if (ret < 0) {

        error_setg(errp, "Failed to remove snapshot from snapshot list");

        return ret;

    }



    

    g_free(sn.id_str);

    g_free(sn.name);



    

    ret = qcow2_update_snapshot_refcount(bs, sn.l1_table_offset,

                                         sn.l1_size, -1);

    if (ret < 0) {

        error_setg(errp, "Failed to free the cluster and L1 table");

        return ret;

    }

    qcow2_free_clusters(bs, sn.l1_table_offset, sn.l1_size * sizeof(uint64_t),

                        QCOW2_DISCARD_SNAPSHOT);



    

    ret = qcow2_update_snapshot_refcount(bs, s->l1_table_offset, s->l1_size, 0);

    if (ret < 0) {

        error_setg(errp, "Failed to update snapshot status in disk");

        return ret;

    }



#ifdef DEBUG_ALLOC

    {

        BdrvCheckResult result = {0};

        qcow2_check_refcounts(bs, &result, 0);

    }

#endif

    return 0;

}