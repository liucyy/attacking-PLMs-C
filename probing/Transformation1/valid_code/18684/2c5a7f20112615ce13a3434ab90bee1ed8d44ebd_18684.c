static int block_save_iterate(QEMUFile *f, void *opaque)

{

    int ret;



    DPRINTF("Enter save live iterate submitted %d transferred %d\n",

            block_mig_state.submitted, block_mig_state.transferred);



    ret = flush_blks(f);

    if (ret) {

        blk_mig_cleanup();

        return ret;

    }



    blk_mig_reset_dirty_cursor();



    

    while ((block_mig_state.submitted +

            block_mig_state.read_done) * BLOCK_SIZE <

           qemu_file_get_rate_limit(f)) {

        if (block_mig_state.bulk_completed == 0) {

            

            if (blk_mig_save_bulked_block(f) == 0) {

                

                block_mig_state.bulk_completed = 1;

            }

        } else {

            ret = blk_mig_save_dirty_block(f, 1);

            if (ret != 0) {

                

                break;

            }

        }

    }

    if (ret < 0) {

        blk_mig_cleanup();

        return ret;

    }



    ret = flush_blks(f);

    if (ret) {

        blk_mig_cleanup();

        return ret;

    }



    qemu_put_be64(f, BLK_MIG_FLAG_EOS);



    return 0;

}