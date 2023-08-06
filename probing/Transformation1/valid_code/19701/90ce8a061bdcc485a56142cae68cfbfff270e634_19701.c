static void bdrv_move_feature_fields(BlockDriverState *bs_dest,

                                     BlockDriverState *bs_src)

{

    

    bs_dest->open_flags         = bs_src->open_flags;



    

    bs_dest->dev_ops            = bs_src->dev_ops;

    bs_dest->dev_opaque         = bs_src->dev_opaque;

    bs_dest->dev                = bs_src->dev;

    bs_dest->guest_block_size   = bs_src->guest_block_size;

    bs_dest->copy_on_read       = bs_src->copy_on_read;



    bs_dest->enable_write_cache = bs_src->enable_write_cache;



    

    memcpy(&bs_dest->throttle_state,

           &bs_src->throttle_state,

           sizeof(ThrottleState));

    bs_dest->throttled_reqs[0]  = bs_src->throttled_reqs[0];

    bs_dest->throttled_reqs[1]  = bs_src->throttled_reqs[1];

    bs_dest->io_limits_enabled  = bs_src->io_limits_enabled;



    

    bs_dest->on_read_error      = bs_src->on_read_error;

    bs_dest->on_write_error     = bs_src->on_write_error;



    

    bs_dest->iostatus_enabled   = bs_src->iostatus_enabled;

    bs_dest->iostatus           = bs_src->iostatus;



    

    bs_dest->dirty_bitmaps      = bs_src->dirty_bitmaps;



    

    bs_dest->refcnt             = bs_src->refcnt;



    

    bs_dest->in_use             = bs_src->in_use;

    bs_dest->job                = bs_src->job;



    

    pstrcpy(bs_dest->device_name, sizeof(bs_dest->device_name),

            bs_src->device_name);

    bs_dest->device_list = bs_src->device_list;



    

    bs_dest->node_list   = bs_src->node_list;

}