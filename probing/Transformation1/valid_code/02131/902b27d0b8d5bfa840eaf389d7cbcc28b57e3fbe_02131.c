int bdrv_open2(BlockDriverState *bs, const char *filename, int flags,
               BlockDriver *drv)
{
    int ret, open_flags;
    char tmp_filename[PATH_MAX];
    char backing_filename[PATH_MAX];
    bs->read_only = 0;
    bs->is_temporary = 0;
    bs->encrypted = 0;
    if (flags & BDRV_O_SNAPSHOT) {
        BlockDriverState *bs1;
        int64_t total_size;
        
        
        bs1 = bdrv_new("");
        if (!bs1) {
            return -ENOMEM;
        }
        if (bdrv_open(bs1, filename, 0) < 0) {
            bdrv_delete(bs1);
            return -1;
        }
        total_size = bdrv_getlength(bs1) >> SECTOR_BITS;
        bdrv_delete(bs1);
        get_tmp_filename(tmp_filename, sizeof(tmp_filename));
        realpath(filename, backing_filename);
        if (bdrv_create(&bdrv_qcow2, tmp_filename,
                        total_size, backing_filename, 0) < 0) {
            return -1;
        }
        filename = tmp_filename;
        bs->is_temporary = 1;
    }
    pstrcpy(bs->filename, sizeof(bs->filename), filename);
    if (flags & BDRV_O_FILE) {
        drv = find_protocol(filename);
        if (!drv)
            return -ENOENT;
    } else {
        if (!drv) {
            drv = find_image_format(filename);
            if (!drv)
                return -1;
        }
    }
    bs->drv = drv;
    bs->opaque = qemu_mallocz(drv->instance_size);
    bs->total_sectors = 0; 
    if (bs->opaque == NULL && drv->instance_size > 0)
        return -1;
    
    if (!(flags & BDRV_O_FILE))
        open_flags = BDRV_O_RDWR | (flags & BDRV_O_DIRECT);
    else
        open_flags = flags & ~(BDRV_O_FILE | BDRV_O_SNAPSHOT);
    ret = drv->bdrv_open(bs, filename, open_flags);
    if (ret == -EACCES && !(flags & BDRV_O_FILE)) {
        ret = drv->bdrv_open(bs, filename, BDRV_O_RDONLY);
        bs->read_only = 1;
    }
    if (ret < 0) {
        qemu_free(bs->opaque);
        bs->opaque = NULL;
        bs->drv = NULL;
        return ret;
    }
    if (drv->bdrv_getlength) {
        bs->total_sectors = bdrv_getlength(bs) >> SECTOR_BITS;
    }
#ifndef _WIN32
    if (bs->is_temporary) {
        unlink(filename);
    }
#endif
    if (bs->backing_file[0] != '\0') {
        
        bs->backing_hd = bdrv_new("");
        if (!bs->backing_hd) {
        fail:
            bdrv_close(bs);
            return -ENOMEM;
        }
        path_combine(backing_filename, sizeof(backing_filename),
                     filename, bs->backing_file);
        if (bdrv_open(bs->backing_hd, backing_filename, 0) < 0)
            goto fail;
    }
    
    bs->media_changed = 1;
    if (bs->change_cb)
        bs->change_cb(bs->change_opaque);
    return 0;
}