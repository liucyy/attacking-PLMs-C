static int dnxhd_decode_dct_block_10_444(const DNXHDContext *ctx,

                                         RowContext *row, int n)

{

    return dnxhd_decode_dct_block(ctx, row, n, 6, 32, 6);

}