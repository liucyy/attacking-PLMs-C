int qcow2_grow_l1_table(BlockDriverState *bs, int min_size)

{

    BDRVQcowState *s = bs->opaque;

    int new_l1_size, new_l1_size2, ret, i;

    uint64_t *new_l1_table;

    int64_t new_l1_table_offset;

    uint8_t data[12];



    new_l1_size = s->l1_size;

    if (min_size <= new_l1_size)

        return 0;

    if (new_l1_size == 0) {

        new_l1_size = 1;

    }

    while (min_size > new_l1_size) {

        new_l1_size = (new_l1_size * 3 + 1) / 2;

    }

#ifdef DEBUG_ALLOC2

    printf("grow l1_table from %d to %d\n", s->l1_size, new_l1_size);

#endif



    new_l1_size2 = sizeof(uint64_t) * new_l1_size;

    new_l1_table = qemu_mallocz(align_offset(new_l1_size2, 512));

    memcpy(new_l1_table, s->l1_table, s->l1_size * sizeof(uint64_t));



    

    BLKDBG_EVENT(bs->file, BLKDBG_L1_GROW_ALLOC_TABLE);

    new_l1_table_offset = qcow2_alloc_clusters(bs, new_l1_size2);

    if (new_l1_table_offset < 0) {

        qemu_free(new_l1_table);

        return new_l1_table_offset;

    }



    BLKDBG_EVENT(bs->file, BLKDBG_L1_GROW_WRITE_TABLE);

    for(i = 0; i < s->l1_size; i++)

        new_l1_table[i] = cpu_to_be64(new_l1_table[i]);

    ret = bdrv_pwrite(bs->file, new_l1_table_offset, new_l1_table, new_l1_size2);

    if (ret != new_l1_size2)

        goto fail;

    for(i = 0; i < s->l1_size; i++)

        new_l1_table[i] = be64_to_cpu(new_l1_table[i]);



    

    BLKDBG_EVENT(bs->file, BLKDBG_L1_GROW_ACTIVATE_TABLE);

    cpu_to_be32w((uint32_t*)data, new_l1_size);

    cpu_to_be64w((uint64_t*)(data + 4), new_l1_table_offset);

    ret = bdrv_pwrite(bs->file, offsetof(QCowHeader, l1_size), data,sizeof(data));

    if (ret != sizeof(data)) {

        goto fail;

    }

    qemu_free(s->l1_table);

    qcow2_free_clusters(bs, s->l1_table_offset, s->l1_size * sizeof(uint64_t));

    s->l1_table_offset = new_l1_table_offset;

    s->l1_table = new_l1_table;

    s->l1_size = new_l1_size;

    return 0;

 fail:

    qemu_free(new_l1_table);

    qcow2_free_clusters(bs, new_l1_table_offset, new_l1_size2);

    return ret < 0 ? ret : -EIO;

}