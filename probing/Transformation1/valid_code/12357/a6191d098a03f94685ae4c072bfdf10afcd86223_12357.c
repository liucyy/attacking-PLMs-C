static void put_subframe(DCAEncContext *c, int subframe)

{

    int i, band, ss, ch;



    

    put_bits(&c->pb, 2, SUBSUBFRAMES -1);



    

    put_bits(&c->pb, 3, 0);



    

    for (ch = 0; ch < c->fullband_channels; ch++)

        for (band = 0; band < DCAENC_SUBBANDS; band++)

            put_bits(&c->pb, 1, 0);



    

    

    for (ch = 0; ch < c->fullband_channels; ch++)

        for (band = 0; band < DCAENC_SUBBANDS; band++)

            put_bits(&c->pb, 5, c->abits[band][ch]);



    if (SUBSUBFRAMES > 1) {

        

        for (ch = 0; ch < c->fullband_channels; ch++)

            for (band = 0; band < DCAENC_SUBBANDS; band++)

                put_bits(&c->pb, 1, 0); 

    }



    

    for (ch = 0; ch < c->fullband_channels; ch++)

        for (band = 0; band < DCAENC_SUBBANDS; band++)

            put_bits(&c->pb, 7, c->scale_factor[band][ch]);



    

    

    

    

    

    



    

    if (c->lfe_channel) {

        for (i = 0; i < DCA_LFE_SAMPLES; i++)

            put_bits(&c->pb, 8, quantize_value(c->downsampled_lfe[i], c->lfe_quant) & 0xff);

        put_bits(&c->pb, 8, c->lfe_scale_factor);

    }



    

    for (ss = 0; ss < SUBSUBFRAMES ; ss++)

        for (ch = 0; ch < c->fullband_channels; ch++)

            for (band = 0; band < DCAENC_SUBBANDS; band++)

                    put_subframe_samples(c, ss, band, ch);



    

    put_bits(&c->pb, 16, 0xffff);

}