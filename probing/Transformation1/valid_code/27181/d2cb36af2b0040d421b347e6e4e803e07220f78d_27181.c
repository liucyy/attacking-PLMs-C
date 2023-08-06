int qcow2_snapshot_create(BlockDriverState *bs, QEMUSnapshotInfo *sn_info)

{

    BDRVQcow2State *s = bs->opaque;

    QCowSnapshot *new_snapshot_list = NULL;

    QCowSnapshot *old_snapshot_list = NULL;

    QCowSnapshot sn1, *sn = &sn1;

    int i, ret;

    uint64_t *l1_table = NULL;

    int64_t l1_table_offset;



    if (s->nb_snapshots >= QCOW_MAX_SNAPSHOTS) {

        return -EFBIG;

    }



    memset(sn, 0, sizeof(*sn));



    

    find_new_snapshot_id(bs, sn_info->id_str, sizeof(sn_info->id_str));



    

    if (find_snapshot_by_id_and_name(bs, sn_info->id_str, NULL) >= 0) {

        return -EEXIST;

    }



    

    sn->id_str = g_strdup(sn_info->id_str);

    sn->name = g_strdup(sn_info->name);



    sn->disk_size = bs->total_sectors * BDRV_SECTOR_SIZE;

    sn->vm_state_size = sn_info->vm_state_size;

    sn->date_sec = sn_info->date_sec;

    sn->date_nsec = sn_info->date_nsec;

    sn->vm_clock_nsec = sn_info->vm_clock_nsec;



    

    l1_table_offset = qcow2_alloc_clusters(bs, s->l1_size * sizeof(uint64_t));

    if (l1_table_offset < 0) {

        ret = l1_table_offset;

        goto fail;

    }



    sn->l1_table_offset = l1_table_offset;

    sn->l1_size = s->l1_size;



    l1_table = g_try_new(uint64_t, s->l1_size);

    if (s->l1_size && l1_table == NULL) {

        ret = -ENOMEM;

        goto fail;

    }



    for(i = 0; i < s->l1_size; i++) {

        l1_table[i] = cpu_to_be64(s->l1_table[i]);

    }



    ret = qcow2_pre_write_overlap_check(bs, 0, sn->l1_table_offset,

                                        s->l1_size * sizeof(uint64_t));

    if (ret < 0) {

        goto fail;

    }



    ret = bdrv_pwrite(bs->file, sn->l1_table_offset, l1_table,

                      s->l1_size * sizeof(uint64_t));

    if (ret < 0) {

        goto fail;

    }



    g_free(l1_table);

    l1_table = NULL;



    

    ret = qcow2_update_snapshot_refcount(bs, s->l1_table_offset, s->l1_size, 1);

    if (ret < 0) {

        goto fail;

    }



    

    new_snapshot_list = g_new(QCowSnapshot, s->nb_snapshots + 1);

    if (s->snapshots) {

        memcpy(new_snapshot_list, s->snapshots,

               s->nb_snapshots * sizeof(QCowSnapshot));

        old_snapshot_list = s->snapshots;

    }

    s->snapshots = new_snapshot_list;

    s->snapshots[s->nb_snapshots++] = *sn;



    ret = qcow2_write_snapshots(bs);

    if (ret < 0) {

        g_free(s->snapshots);

        s->snapshots = old_snapshot_list;

        s->nb_snapshots--;

        goto fail;

    }



    g_free(old_snapshot_list);



    

    qcow2_discard_clusters(bs, qcow2_vm_state_offset(s),

                           align_offset(sn->vm_state_size, s->cluster_size)

                                >> BDRV_SECTOR_BITS,

                           QCOW2_DISCARD_NEVER, false);



#ifdef DEBUG_ALLOC

    {

      BdrvCheckResult result = {0};

      qcow2_check_refcounts(bs, &result, 0);

    }

#endif

    return 0;



fail:

    g_free(sn->id_str);

    g_free(sn->name);

    g_free(l1_table);



    return ret;

}