static int alloc_refcount_block(BlockDriverState *bs,
                                int64_t cluster_index, void **refcount_block)
{
    BDRVQcow2State *s = bs->opaque;
    unsigned int refcount_table_index;
    int64_t ret;
    BLKDBG_EVENT(bs->file, BLKDBG_REFBLOCK_ALLOC);
    
    refcount_table_index = cluster_index >> s->refcount_block_bits;
    if (refcount_table_index < s->refcount_table_size) {
        uint64_t refcount_block_offset =
            s->refcount_table[refcount_table_index] & REFT_OFFSET_MASK;
        
        if (refcount_block_offset) {
            if (offset_into_cluster(s, refcount_block_offset)) {
                qcow2_signal_corruption(bs, true, -1, -1, "Refblock offset %#"
                                        PRIx64 " unaligned (reftable index: "
                                        "%#x)", refcount_block_offset,
                                        refcount_table_index);
             return load_refcount_block(bs, refcount_block_offset,
                                        refcount_block);
    
    *refcount_block = NULL;
    
    ret = qcow2_cache_flush(bs, s->l2_table_cache);
    if (ret < 0) {
        return ret;
    
    int64_t new_block = alloc_clusters_noref(bs, s->cluster_size);
    if (new_block < 0) {
        return new_block;
#ifdef DEBUG_ALLOC2
    fprintf(stderr, "qcow2: Allocate refcount block %d for %" PRIx64
        " at %" PRIx64 "\n",
        refcount_table_index, cluster_index << s->cluster_bits, new_block);
#endif
    if (in_same_refcount_block(s, new_block, cluster_index << s->cluster_bits)) {
        
        ret = qcow2_cache_get_empty(bs, s->refcount_block_cache, new_block,
                                    refcount_block);
        if (ret < 0) {
            goto fail;
        memset(*refcount_block, 0, s->cluster_size);
        
        int block_index = (new_block >> s->cluster_bits) &
            (s->refcount_block_size - 1);
        s->set_refcount(*refcount_block, block_index, 1);
    } else {
        
        ret = update_refcount(bs, new_block, s->cluster_size, 1, false,
                              QCOW2_DISCARD_NEVER);
        if (ret < 0) {
            goto fail;
        ret = qcow2_cache_flush(bs, s->refcount_block_cache);
        if (ret < 0) {
            goto fail;
        
        ret = qcow2_cache_get_empty(bs, s->refcount_block_cache, new_block,
                                    refcount_block);
        if (ret < 0) {
            goto fail;
        memset(*refcount_block, 0, s->cluster_size);
    
    BLKDBG_EVENT(bs->file, BLKDBG_REFBLOCK_ALLOC_WRITE);
    qcow2_cache_entry_mark_dirty(bs, s->refcount_block_cache, *refcount_block);
    ret = qcow2_cache_flush(bs, s->refcount_block_cache);
    if (ret < 0) {
        goto fail;
    
    if (refcount_table_index < s->refcount_table_size) {
        uint64_t data64 = cpu_to_be64(new_block);
        BLKDBG_EVENT(bs->file, BLKDBG_REFBLOCK_ALLOC_HOOKUP);
        ret = bdrv_pwrite_sync(bs->file,
            s->refcount_table_offset + refcount_table_index * sizeof(uint64_t),
            &data64, sizeof(data64));
        if (ret < 0) {
            goto fail;
        s->refcount_table[refcount_table_index] = new_block;
        
        s->max_refcount_table_index =
            MAX(s->max_refcount_table_index, refcount_table_index);
        
        return -EAGAIN;
    qcow2_cache_put(bs, s->refcount_block_cache, refcount_block);
    
    BLKDBG_EVENT(bs->file, BLKDBG_REFTABLE_GROW);
    
    uint64_t blocks_used = DIV_ROUND_UP(MAX(cluster_index + 1,
                                            (new_block >> s->cluster_bits) + 1),
                                        s->refcount_block_size);
    
    uint64_t meta_offset = (blocks_used * s->refcount_block_size) *
        s->cluster_size;
    ret = qcow2_refcount_area(bs, meta_offset, 0, false,
                              refcount_table_index, new_block);
    if (ret < 0) {
        return ret;
    ret = load_refcount_block(bs, new_block, refcount_block);
    if (ret < 0) {
        return ret;
    
    return -EAGAIN;
fail:
    if (*refcount_block != NULL) {
        qcow2_cache_put(bs, s->refcount_block_cache, refcount_block);
    return ret;