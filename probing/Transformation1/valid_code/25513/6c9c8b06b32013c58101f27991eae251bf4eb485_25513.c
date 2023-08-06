static int interp(RA144Context *ractx, int16_t *out, int block_num,

                  int copyold, int energy)

{

    int work[10];

    int a = block_num + 1;

    int b = NBLOCKS - a;

    int x;



    

    

    for (x=0; x<30; x++)

        out[x] = (a * ractx->lpc_coef[0][x] + b * ractx->lpc_coef[1][x])>> 2;



    if (eval_refl(work, out, ractx)) {

        

        

        int_to_int16(out, ractx->lpc_coef[copyold]);

        return rescale_rms(ractx->lpc_refl_rms[copyold], energy);

    } else {

        return rescale_rms(rms(work), energy);

    }

}