static void decode_ac_filter(WmallDecodeCtx *s)

{

    int i;

    s->acfilter_order = get_bits(&s->gb, 4) + 1;

    s->acfilter_scaling = get_bits(&s->gb, 4);



    for(i = 0; i < s->acfilter_order; i++) {

	s->acfilter_coeffs[i] = get_bits(&s->gb, s->acfilter_scaling) + 1;

    }

}