static int decode_block(ALSDecContext *ctx, ALSBlockData *bd)

{

    unsigned int smp;



    

    if (*bd->const_block)

        decode_const_block_data(ctx, bd);

    else if (decode_var_block_data(ctx, bd))

        return -1;



    



    if (*bd->shift_lsbs)

        for (smp = 0; smp < bd->block_length; smp++)

            bd->raw_samples[smp] <<= *bd->shift_lsbs;



    return 0;

}