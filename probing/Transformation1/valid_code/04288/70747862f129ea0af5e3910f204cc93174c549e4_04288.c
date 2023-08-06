static int vdi_create(const char *filename, QemuOpts *opts, Error **errp)

{

    int fd;

    int result = 0;

    uint64_t bytes = 0;

    uint32_t blocks;

    size_t block_size = DEFAULT_CLUSTER_SIZE;

    uint32_t image_type = VDI_TYPE_DYNAMIC;

    VdiHeader header;

    size_t i;

    size_t bmap_size;

    bool nocow = false;



    logout("\n");



    

    bytes = qemu_opt_get_size_del(opts, BLOCK_OPT_SIZE, 0);

#if defined(CONFIG_VDI_BLOCK_SIZE)

    

    block_size = qemu_opt_get_size_del(opts,

                                       BLOCK_OPT_CLUSTER_SIZE,

                                       DEFAULT_CLUSTER_SIZE);

#endif

#if defined(CONFIG_VDI_STATIC_IMAGE)

    if (qemu_opt_get_bool_del(opts, BLOCK_OPT_STATIC, false)) {

        image_type = VDI_TYPE_STATIC;

    }

#endif

    nocow = qemu_opt_get_bool_del(opts, BLOCK_OPT_NOCOW, false);



    if (bytes > VDI_DISK_SIZE_MAX) {

        result = -ENOTSUP;

        error_setg(errp, "Unsupported VDI image size (size is 0x%" PRIx64

                          ", max supported is 0x%" PRIx64 ")",

                          bytes, VDI_DISK_SIZE_MAX);

        goto exit;

    }



    fd = qemu_open(filename,

                   O_WRONLY | O_CREAT | O_TRUNC | O_BINARY | O_LARGEFILE,

                   0644);

    if (fd < 0) {

        result = -errno;

        goto exit;

    }



    if (nocow) {

#ifdef __linux__

        

        int attr;

        if (ioctl(fd, FS_IOC_GETFLAGS, &attr) == 0) {

            attr |= FS_NOCOW_FL;

            ioctl(fd, FS_IOC_SETFLAGS, &attr);

        }

#endif

    }



    

    blocks = (bytes + block_size - 1) / block_size;



    bmap_size = blocks * sizeof(uint32_t);

    bmap_size = ((bmap_size + SECTOR_SIZE - 1) & ~(SECTOR_SIZE -1));



    memset(&header, 0, sizeof(header));

    pstrcpy(header.text, sizeof(header.text), VDI_TEXT);

    header.signature = VDI_SIGNATURE;

    header.version = VDI_VERSION_1_1;

    header.header_size = 0x180;

    header.image_type = image_type;

    header.offset_bmap = 0x200;

    header.offset_data = 0x200 + bmap_size;

    header.sector_size = SECTOR_SIZE;

    header.disk_size = bytes;

    header.block_size = block_size;

    header.blocks_in_image = blocks;

    if (image_type == VDI_TYPE_STATIC) {

        header.blocks_allocated = blocks;

    }

    uuid_generate(header.uuid_image);

    uuid_generate(header.uuid_last_snap);

    

#if defined(CONFIG_VDI_DEBUG)

    vdi_header_print(&header);

#endif

    vdi_header_to_le(&header);

    if (write(fd, &header, sizeof(header)) < 0) {

        result = -errno;

        goto close_and_exit;

    }



    if (bmap_size > 0) {

        uint32_t *bmap = g_malloc0(bmap_size);

        for (i = 0; i < blocks; i++) {

            if (image_type == VDI_TYPE_STATIC) {

                bmap[i] = i;

            } else {

                bmap[i] = VDI_UNALLOCATED;

            }

        }

        if (write(fd, bmap, bmap_size) < 0) {

            result = -errno;

            g_free(bmap);

            goto close_and_exit;

        }

        g_free(bmap);

    }



    if (image_type == VDI_TYPE_STATIC) {

        if (ftruncate(fd, sizeof(header) + bmap_size + blocks * block_size)) {

            result = -errno;

            goto close_and_exit;

        }

    }



close_and_exit:

    if ((close(fd) < 0) && !result) {

        result = -errno;

    }



exit:

    return result;

}