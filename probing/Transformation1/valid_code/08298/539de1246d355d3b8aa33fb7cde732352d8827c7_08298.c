static int block_save_live(Monitor *mon, QEMUFile *f, int stage, void *opaque)

{

    int ret;



    DPRINTF("Enter save live stage %d submitted %d transferred %d\n",

            stage, block_mig_state.submitted, block_mig_state.transferred);



    if (stage < 0) {

        blk_mig_cleanup(mon);

        return 0;

    }



    if (block_mig_state.blk_enable != 1) {

        

        qemu_put_be64(f, BLK_MIG_FLAG_EOS);

        return 1;

    }



    if (stage == 1) {

        init_blk_migration(mon, f);



        

        set_dirty_tracking(1);

    }



    flush_blks(f);



    ret = qemu_file_get_error(f);

    if (ret) {

        blk_mig_cleanup(mon);

        return ret;

    }



    blk_mig_reset_dirty_cursor();



    if (stage == 2) {

        

        while ((block_mig_state.submitted +

                block_mig_state.read_done) * BLOCK_SIZE <

               qemu_file_get_rate_limit(f)) {

            if (block_mig_state.bulk_completed == 0) {

                

                if (blk_mig_save_bulked_block(mon, f) == 0) {

                    

                    block_mig_state.bulk_completed = 1;

                }

            } else {

                if (blk_mig_save_dirty_block(mon, f, 1) == 0) {

                    

                    break;

                }

            }

        }



        flush_blks(f);



        ret = qemu_file_get_error(f);

        if (ret) {

            blk_mig_cleanup(mon);

            return ret;

        }

    }



    if (stage == 3) {

        

        assert(block_mig_state.submitted == 0);



        while (blk_mig_save_dirty_block(mon, f, 0) != 0);

        blk_mig_cleanup(mon);



        

        qemu_put_be64(f, (100 << BDRV_SECTOR_BITS) | BLK_MIG_FLAG_PROGRESS);



        ret = qemu_file_get_error(f);

        if (ret) {

            return ret;

        }



        monitor_printf(mon, "Block migration completed\n");

    }



    qemu_put_be64(f, BLK_MIG_FLAG_EOS);



    return ((stage == 2) && is_stage2_completed());

}