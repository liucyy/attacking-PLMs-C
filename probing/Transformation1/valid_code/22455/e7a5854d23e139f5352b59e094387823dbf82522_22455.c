static int amr_wb_encode_frame(AVCodecContext *avctx,

                            unsigned char *frame, int buf_size, void *data)

{

    AMRWBContext *s;

    int size;

    s = (AMRWBContext*) avctx->priv_data;

    s->mode=getWBBitrateMode(avctx->bit_rate);

    size = E_IF_encode(s->state, s->mode, data, frame, s->allow_dtx);

    return size;

}