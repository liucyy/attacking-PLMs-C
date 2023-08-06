static void bdrv_qed_drain(BlockDriverState *bs)

{

    BDRVQEDState *s = bs->opaque;



    

    qed_cancel_need_check_timer(s);

    qed_plug_allocating_write_reqs(s);

    bdrv_aio_flush(s->bs, qed_clear_need_check, s);

}