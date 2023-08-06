static void block_set_params(const MigrationParams *params, void *opaque)

{

    block_mig_state.blk_enable = params->blk;

    block_mig_state.shared_base = params->shared;



    

    block_mig_state.blk_enable |= params->shared;

}