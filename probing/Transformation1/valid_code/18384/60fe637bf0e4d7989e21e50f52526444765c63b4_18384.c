static void blk_send(QEMUFile *f, BlkMigBlock * blk)

{

    int len;

    uint64_t flags = BLK_MIG_FLAG_DEVICE_BLOCK;



    if (block_mig_state.zero_blocks &&

        buffer_is_zero(blk->buf, BLOCK_SIZE)) {

        flags |= BLK_MIG_FLAG_ZERO_BLOCK;

    }



    

    qemu_put_be64(f, (blk->sector << BDRV_SECTOR_BITS)

                     | flags);



    

    len = strlen(bdrv_get_device_name(blk->bmds->bs));

    qemu_put_byte(f, len);

    qemu_put_buffer(f, (uint8_t *)bdrv_get_device_name(blk->bmds->bs), len);



    

    if (flags & BLK_MIG_FLAG_ZERO_BLOCK) {

        qemu_fflush(f);

        return;

    }



    qemu_put_buffer(f, blk->buf, BLOCK_SIZE);

}