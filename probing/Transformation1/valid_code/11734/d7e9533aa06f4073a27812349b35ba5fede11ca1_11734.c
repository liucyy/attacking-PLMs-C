static void convert_matrix(int *qmat, UINT16 *qmat16, const UINT16 *quant_matrix, int qscale)

{

    int i;



    if (av_fdct == jpeg_fdct_ifast) {

        for(i=0;i<64;i++) {

            

            

            

            

            

            qmat[block_permute_op(i)] = (int)((UINT64_C(1) << (QMAT_SHIFT + 11)) / 

                            (aanscales[i] * qscale * quant_matrix[block_permute_op(i)]));

        }

    } else {

        for(i=0;i<64;i++) {

            

            qmat[i]   = (1 << QMAT_SHIFT_MMX) / (qscale * quant_matrix[i]);

            qmat16[i] = (1 << QMAT_SHIFT_MMX) / (qscale * quant_matrix[block_permute_op(i)]);

        }

    }

}