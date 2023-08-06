vdi_co_pwritev(BlockDriverState *bs, uint64_t offset, uint64_t bytes,

               QEMUIOVector *qiov, int flags)

{

    BDRVVdiState *s = bs->opaque;

    QEMUIOVector local_qiov;

    uint32_t bmap_entry;

    uint32_t block_index;

    uint32_t offset_in_block;

    uint32_t n_bytes;

    uint32_t bmap_first = VDI_UNALLOCATED;

    uint32_t bmap_last = VDI_UNALLOCATED;

    uint8_t *block = NULL;

    uint64_t bytes_done = 0;

    int ret = 0;



    logout("\n");



    qemu_iovec_init(&local_qiov, qiov->niov);



    while (ret >= 0 && bytes > 0) {

        block_index = offset / s->block_size;

        offset_in_block = offset % s->block_size;

        n_bytes = MIN(bytes, s->block_size - offset_in_block);



        logout("will write %u bytes starting at offset %" PRIu64 "\n",

               n_bytes, offset);



        

        bmap_entry = le32_to_cpu(s->bmap[block_index]);

        if (!VDI_IS_ALLOCATED(bmap_entry)) {

            

            uint64_t data_offset;

            bmap_entry = s->header.blocks_allocated;

            s->bmap[block_index] = cpu_to_le32(bmap_entry);

            s->header.blocks_allocated++;

            data_offset = s->header.offset_data +

                          (uint64_t)bmap_entry * s->block_size;

            if (block == NULL) {

                block = g_malloc(s->block_size);

                bmap_first = block_index;

            }

            bmap_last = block_index;

            

            memset(block, 0, offset_in_block);

            qemu_iovec_to_buf(qiov, bytes_done, block + offset_in_block,

                              n_bytes);

            memset(block + offset_in_block + n_bytes, 0,

                   s->block_size - n_bytes - offset_in_block);



            

            qemu_co_mutex_lock(&s->write_lock);

            ret = bdrv_pwrite(bs->file, data_offset, block, s->block_size);

            qemu_co_mutex_unlock(&s->write_lock);

        } else {

            uint64_t data_offset = s->header.offset_data +

                                   (uint64_t)bmap_entry * s->block_size +

                                   offset_in_block;

            qemu_co_mutex_lock(&s->write_lock);

            

            qemu_co_mutex_unlock(&s->write_lock);



            qemu_iovec_reset(&local_qiov);

            qemu_iovec_concat(&local_qiov, qiov, bytes_done, n_bytes);



            ret = bdrv_co_pwritev(bs->file, data_offset, n_bytes,

                                  &local_qiov, 0);

        }



        bytes -= n_bytes;

        offset += n_bytes;

        bytes_done += n_bytes;



        logout("%u bytes written\n", n_bytes);

    }



    qemu_iovec_destroy(&local_qiov);



    logout("finished data write\n");

    if (ret < 0) {

        return ret;

    }



    if (block) {

        

        VdiHeader *header = (VdiHeader *) block;

        uint8_t *base;

        uint64_t offset;

        uint32_t n_sectors;



        logout("now writing modified header\n");

        assert(VDI_IS_ALLOCATED(bmap_first));

        *header = s->header;

        vdi_header_to_le(header);

        ret = bdrv_write(bs->file, 0, block, 1);

        g_free(block);

        block = NULL;



        if (ret < 0) {

            return ret;

        }



        logout("now writing modified block map entry %u...%u\n",

               bmap_first, bmap_last);

        

        bmap_first /= (SECTOR_SIZE / sizeof(uint32_t));

        bmap_last /= (SECTOR_SIZE / sizeof(uint32_t));

        n_sectors = bmap_last - bmap_first + 1;

        offset = s->bmap_sector + bmap_first;

        base = ((uint8_t *)&s->bmap[0]) + bmap_first * SECTOR_SIZE;

        logout("will write %u block map sectors starting from entry %u\n",

               n_sectors, bmap_first);

        ret = bdrv_write(bs->file, offset, base, n_sectors);

    }



    return ret;

}