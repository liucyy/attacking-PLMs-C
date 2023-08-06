static int decode_sequence_header_adv(VC1Context *v, GetBitContext *gb)

{

    v->res_rtm_flag = 1;

    v->level = get_bits(gb, 3);

    if(v->level >= 5)

    {

        av_log(v->s.avctx, AV_LOG_ERROR, "Reserved LEVEL %i\n",v->level);

    }

    v->chromaformat = get_bits(gb, 2);

    if (v->chromaformat != 1)

    {

        av_log(v->s.avctx, AV_LOG_ERROR,

               "Only 4:2:0 chroma format supported\n");

        return -1;

    }



    

    v->frmrtq_postproc = get_bits(gb, 3); 

    

    v->bitrtq_postproc = get_bits(gb, 5); 

    v->postprocflag = get_bits(gb, 1); 



    v->s.avctx->coded_width = (get_bits(gb, 12) + 1) << 1;

    v->s.avctx->coded_height = (get_bits(gb, 12) + 1) << 1;

    v->broadcast = get_bits1(gb);

    v->interlace = get_bits1(gb);

    if(v->interlace){

        av_log(v->s.avctx, AV_LOG_ERROR, "Interlaced mode not supported (yet)\n");

        return -1;

    }

    v->tfcntrflag = get_bits1(gb);

    v->finterpflag = get_bits1(gb);

    get_bits1(gb); 

    v->psf = get_bits1(gb);

    if(v->psf) { 

        av_log(v->s.avctx, AV_LOG_ERROR, "Progressive Segmented Frame mode: not supported (yet)\n");

        return -1;

    }

    if(get_bits1(gb)) { 

        int w, h, ar = 0;

        av_log(v->s.avctx, AV_LOG_INFO, "Display extended info:\n");

        w = get_bits(gb, 14);

        h = get_bits(gb, 14);

        av_log(v->s.avctx, AV_LOG_INFO, "Display dimensions: %ix%i\n", w, h);

        

        if(get_bits1(gb))

            ar = get_bits(gb, 4);

        if(ar == 15) {

            w = get_bits(gb, 8);

            h = get_bits(gb, 8);

        }



        if(get_bits1(gb)){ 

            if(get_bits1(gb)) {

                get_bits(gb, 16);

            } else {

                get_bits(gb, 8);

                get_bits(gb, 4);

            }

        }



        if(get_bits1(gb)){

            v->color_prim = get_bits(gb, 8);

            v->transfer_char = get_bits(gb, 8);

            v->matrix_coef = get_bits(gb, 8);

        }

    }



    v->hrd_param_flag = get_bits1(gb);

    if(v->hrd_param_flag) {

        int i;

        v->hrd_num_leaky_buckets = get_bits(gb, 5);

        get_bits(gb, 4); 

        get_bits(gb, 4); 

        for(i = 0; i < v->hrd_num_leaky_buckets; i++) {

            get_bits(gb, 16); 

            get_bits(gb, 16); 

        }

    }

    return 0;

}