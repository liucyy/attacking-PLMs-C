static void spr_write_40x_sler (void *opaque, int sprn)

{

    DisasContext *ctx = opaque;



    gen_op_store_40x_sler();

    

    RET_STOP(ctx);

}