static bool bdrv_exceed_bps_limits(BlockDriverState *bs, int nb_sectors,

                 bool is_write, double elapsed_time, uint64_t *wait)

{

    uint64_t bps_limit = 0;

    double   bytes_limit, bytes_base, bytes_res;

    double   slice_time, wait_time;



    if (bs->io_limits.bps[BLOCK_IO_LIMIT_TOTAL]) {

        bps_limit = bs->io_limits.bps[BLOCK_IO_LIMIT_TOTAL];

    } else if (bs->io_limits.bps[is_write]) {

        bps_limit = bs->io_limits.bps[is_write];

    } else {

        if (wait) {

            *wait = 0;

        }



        return false;

    }



    slice_time = bs->slice_end - bs->slice_start;

    slice_time /= (NANOSECONDS_PER_SECOND);

    bytes_limit = bps_limit * slice_time;

    bytes_base  = bs->nr_bytes[is_write] - bs->io_base.bytes[is_write];

    if (bs->io_limits.bps[BLOCK_IO_LIMIT_TOTAL]) {

        bytes_base += bs->nr_bytes[!is_write] - bs->io_base.bytes[!is_write];

    }



    

    bytes_res   = (unsigned) nb_sectors * BDRV_SECTOR_SIZE;



    if (bytes_base + bytes_res <= bytes_limit) {

        if (wait) {

            *wait = 0;

        }



        return false;

    }



    

    wait_time = (bytes_base + bytes_res) / bps_limit - elapsed_time;



    

    bs->slice_time = wait_time * BLOCK_IO_SLICE_TIME * 10;

    bs->slice_end += bs->slice_time - 3 * BLOCK_IO_SLICE_TIME;

    if (wait) {

        *wait = wait_time * BLOCK_IO_SLICE_TIME * 10;

    }



    return true;

}