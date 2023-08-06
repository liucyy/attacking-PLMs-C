static void commit_complete(BlockJob *job, void *opaque)
{
    CommitBlockJob *s = container_of(job, CommitBlockJob, common);
    CommitCompleteData *data = opaque;
    BlockDriverState *active = s->active;
    BlockDriverState *top = blk_bs(s->top);
    BlockDriverState *base = blk_bs(s->base);
    BlockDriverState *overlay_bs = bdrv_find_overlay(active, s->commit_top_bs);
    int ret = data->ret;
    bool remove_commit_top_bs = false;
    
    bdrv_ref(top);
    bdrv_ref(overlay_bs);
    
    blk_unref(s->base);
    if (!block_job_is_cancelled(&s->common) && ret == 0) {
        
        ret = bdrv_drop_intermediate(active, s->commit_top_bs, base,
                                     s->backing_file_str);
    } else if (overlay_bs) {
        
        remove_commit_top_bs = true;
    }
    
    if (s->base_flags != bdrv_get_flags(base)) {
        bdrv_reopen(base, s->base_flags, NULL);
    }
    if (overlay_bs && s->orig_overlay_flags != bdrv_get_flags(overlay_bs)) {
        bdrv_reopen(overlay_bs, s->orig_overlay_flags, NULL);
    }
    g_free(s->backing_file_str);
    blk_unref(s->top);
    block_job_completed(&s->common, ret);
    g_free(data);
    
    if (remove_commit_top_bs) {
        bdrv_set_backing_hd(overlay_bs, top, &error_abort);
    }
}