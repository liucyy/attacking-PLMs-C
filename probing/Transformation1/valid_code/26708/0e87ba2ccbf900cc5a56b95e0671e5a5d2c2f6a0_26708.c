static int vdi_open(BlockDriverState *bs, int flags)

{

    BDRVVdiState *s = bs->opaque;

    VdiHeader header;

    size_t bmap_size;

    int ret;



    logout("\n");



    ret = bdrv_read(bs->file, 0, (uint8_t *)&header, 1);

    if (ret < 0) {

        goto fail;

    }



    vdi_header_to_cpu(&header);

#if defined(CONFIG_VDI_DEBUG)

    vdi_header_print(&header);

#endif



    if (header.disk_size % SECTOR_SIZE != 0) {

        

        logout("odd disk size %" PRIu64 " B, round up\n", header.disk_size);

        header.disk_size += SECTOR_SIZE - 1;

        header.disk_size &= ~(SECTOR_SIZE - 1);

    }



    if (header.version != VDI_VERSION_1_1) {

        logout("unsupported version %u.%u\n",

               header.version >> 16, header.version & 0xffff);

        ret = -ENOTSUP;

        goto fail;

    } else if (header.offset_bmap % SECTOR_SIZE != 0) {

        

        logout("unsupported block map offset 0x%x B\n", header.offset_bmap);

        ret = -ENOTSUP;

        goto fail;

    } else if (header.offset_data % SECTOR_SIZE != 0) {

        

        logout("unsupported data offset 0x%x B\n", header.offset_data);

        ret = -ENOTSUP;

        goto fail;

    } else if (header.sector_size != SECTOR_SIZE) {

        logout("unsupported sector size %u B\n", header.sector_size);

        ret = -ENOTSUP;

        goto fail;

    } else if (header.block_size != 1 * MiB) {

        logout("unsupported block size %u B\n", header.block_size);

        ret = -ENOTSUP;

        goto fail;

    } else if (header.disk_size >

               (uint64_t)header.blocks_in_image * header.block_size) {

        logout("unsupported disk size %" PRIu64 " B\n", header.disk_size);

        ret = -ENOTSUP;

        goto fail;

    } else if (!uuid_is_null(header.uuid_link)) {

        logout("link uuid != 0, unsupported\n");

        ret = -ENOTSUP;

        goto fail;

    } else if (!uuid_is_null(header.uuid_parent)) {

        logout("parent uuid != 0, unsupported\n");

        ret = -ENOTSUP;

        goto fail;

    }



    bs->total_sectors = header.disk_size / SECTOR_SIZE;



    s->block_size = header.block_size;

    s->block_sectors = header.block_size / SECTOR_SIZE;

    s->bmap_sector = header.offset_bmap / SECTOR_SIZE;

    s->header = header;



    bmap_size = header.blocks_in_image * sizeof(uint32_t);

    bmap_size = (bmap_size + SECTOR_SIZE - 1) / SECTOR_SIZE;

    if (bmap_size > 0) {

        s->bmap = g_malloc(bmap_size * SECTOR_SIZE);

    }

    ret = bdrv_read(bs->file, s->bmap_sector, (uint8_t *)s->bmap, bmap_size);

    if (ret < 0) {

        goto fail_free_bmap;

    }



    

    error_set(&s->migration_blocker,

              QERR_BLOCK_FORMAT_FEATURE_NOT_SUPPORTED,

              "vdi", bs->device_name, "live migration");

    migrate_add_blocker(s->migration_blocker);



    return 0;



 fail_free_bmap:

    g_free(s->bmap);



 fail:

    return ret;

}