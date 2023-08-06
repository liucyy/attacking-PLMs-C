static int adx_decode_init(AVCodecContext * avctx)

{

	ADXContext *c = avctx->priv_data;





	c->prev[0].s1 = 0;

	c->prev[0].s2 = 0;

	c->prev[1].s1 = 0;

	c->prev[1].s2 = 0;

	c->header_parsed = 0;

	c->in_temp = 0;

	return 0;

}